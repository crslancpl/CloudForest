#include "EditArea.h"

#include "SearchReplaceDialog.h"
#include "LangPanel.h"
#include "DiagnosticPanel.h"
#include "Popovers.h"
#include "datatypes/common.h"
#include "datatypes/lsp.h"
#include "datatypes/file.h"
#include "Gui_if.h"
#include "editarea/CompletionTool.h"
#include "editarea/DiagnosticTool.h"
#include "pythonbackend/editarea/editarea_class_Py.h"
#include "src/filemanagement/FileOperation.h"
#include "src/filemanagement/FileReader.h"
#include "components/TextArea.h"
#include "style/Style.h"
#include "src/languages/LanguageManager_if.h"
#include "src/session/EditAreaData.h"
#include "src/session/SessionEvent.h"
#include "toolset/event/Event.h"
#include "toolset/syntaxprovider/syntax_provider.h"
#include "toolset/tools/Tool.h"
#include "pythonbackend/editarea/editarea_mod_Py.h"

#include <cstdio>
#include <cstring>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <memory>
#include <string>

/*
 * Yupedef
 */

typedef void (*EditAreaBasicCallback)(EditArea*);
typedef void (*EditAreaLangChangedCallback)(EditArea*,Language*,Language*);
typedef void (*CompletionRequestedCallback)(EditArea*, const ZPosition&);

/*
 * EditArea class
 */

static EditArea *saving_editarea;// cache the edit area that is waiting for saving

static bool OnKeyInput(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, EditArea* parent){
    return parent->KeyInput(keyval, keycode, state);
}

static void OnUnfocused(GtkEventControllerFocus* self, EditArea* parent){
    parent->Unfocused();
}

static void OnMouseMoved(GtkEventControllerMotion* self, double x, double y, EditArea* parent){
    parent->MouseMoved(x, y);
}

static void OnCursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *parent){
    parent->CursorMovedByKey();
}

static void OnTextInserted(GtkTextBuffer* buffer, GtkTextIter* itr, char* text, long int length, EditArea* parent){
    parent->TextInserted(itr, text, length);
}

static void OnTextDeleted (GtkTextBuffer* buffer, GtkTextIter* start, GtkTextIter* end, EditArea* parent){
    parent->TextDeleted(start, end);
}

static void OnTextChanged (GtkTextBuffer* buffer, EditArea* parent){
    parent->TextChanged();
}

static void OnCursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *parent){
    parent->CursorPosChanged();
}

static void OnSaveButtonClicked(GtkButton *self, EditArea* parent){
    parent->Save();
}

static void OnLangButtonClicked(GtkButton *self, EditArea *parent){
    parent->LangButtonClicked();
}

static void OnDiagnosticButtonClicked(GtkButton *self, EditArea *parent){
    parent->DiagnosticButtonClicked();
}

static void OnFileSaved(FileData* file){
    saving_editarea->FileSaved(file);
    saving_editarea = nullptr;
}

EditArea::EditArea(FileData* file){
    /* Load gui */
    m_eventMap = {
        {EditArea::CLOSED, SimpleEvent()},
        {EditArea::COMPLETION_REQUESTED, SimpleEvent()},
        {EditArea::CURSOR_MOVED, SimpleEvent()},
        {EditArea::FILE_DATA_CHANGED, SimpleEvent()},
        {EditArea::FILE_SAVED, SimpleEvent()},
        {EditArea::LANG_CHANGED, SimpleEvent()},
        {EditArea::TEXT_CHANGED, SimpleEvent()}
    };

    this->LoadGui();
    this->LoadFile(file);

    m_diagnosticPopover = new DiagnosticPopover(m_textView);// freed on EditArea deleted

    m_completionTool = std::make_unique<CompletionTool>(*this);

    m_diagnosticTool = std::make_unique<DiagnosticTool>(*this);
    m_diagnosticTool->OnCleared([=](){
        bool r = m_mutex.try_lock();
        m_currentDiagnosticRange.start.line = 0;
        m_currentDiagnosticRange.start.column = 0;
        m_currentDiagnosticRange.end.line = 0;
        m_currentDiagnosticRange.end.column = 0;

        gtk_text_buffer_get_bounds(m_textViewBuffer, &m_startItr, &m_endItr);
        for (const char* tag : style::GetTextTagNamesForDiagnostic()){
            gtk_text_buffer_remove_tag_by_name(m_textViewBuffer, tag, &m_startItr, &m_endItr);
        };
        m_mutex.unlock();
    });

    m_diagnosticTool->OnUpdated([=](int error, int warning, int info, int hint){
        bool r = m_mutex.try_lock();
        std::string s =
            "<span color=\"red\">e" +
            std::to_string(error) +
            "</span> <span color=\"yellow\">w" +
            std::to_string(warning) +
            "</span> <span color=\"greenyellow\">i" +
            std::to_string(info) +
            "</span> <span color=\"cyan\">h" +
            std::to_string(hint) +
            "</span>";

        gtk_label_set_markup(m_diagnButLabel, s.c_str());
        m_mutex.unlock();
    });

    /* Initialize variables */
    m_cursorIndex = 0;
    m_isCurMovedByKey = false;

    /* Set properties */
    gtk_button_set_label(m_saveBut, "Saved");

    this->LoadCursorPos();
    this->ConnectSignals();

    if(m_language == nullptr){
        SetLanguage(langmanager::FindByName("Unknown"));
    }

    m_py_EditArea = py_EditArea_create_object(this);

    const SimpleEvent &event = GetEvent(session::EDITAREA_CREATED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((EditAreaBasicCallback)callback)(this);
    }
}

EditArea::~EditArea(){
    /*
     * remove data
     */

    const SimpleEvent &event =  m_eventMap.at(EditArea::CLOSED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((EditAreaBasicCallback)callback)(this);
    }

    m_editingFile->editArea = nullptr;
}

//private
void EditArea::LoadGui(){
    /* Loading EditArea from UI/EditArea.ui */
    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/EditArea.ui");

    /* Binding */
    m_baseGrid = GTK_GRID(gtk_builder_get_object(builder, "base-grid"));
    // FileInfo panel(top)
    m_locationBut = GTK_BUTTON(gtk_builder_get_object(builder,"location-btn"));
    m_saveBut = GTK_BUTTON(gtk_builder_get_object(builder, "save-btn"));
    // Misc panel(bottom)
    m_outlineBut = GTK_BUTTON(gtk_builder_get_object(builder, "outline-btn"));
    m_diagnBut = GTK_BUTTON(gtk_builder_get_object(builder, "diagn-btn"));
    m_diagnButLabel = GTK_LABEL(gtk_builder_get_object(builder, "diagn-btn-label"));
    m_langBut = GTK_BUTTON(gtk_builder_get_object(builder, "lang-btn"));
    m_cursorPosBut = GTK_BUTTON(gtk_builder_get_object(builder, "cursor-pos-btn"));

    m_keyDownEventCtrl = gtk_event_controller_key_new();
    m_focusEventCtrl = gtk_event_controller_focus_new();
    m_mouseMovedEventCtrl = gtk_event_controller_motion_new();

    gtk_widget_set_has_tooltip(GTK_WIDGET(m_saveBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(m_saveBut), "Save");
    gtk_widget_set_has_tooltip(GTK_WIDGET(m_locationBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(m_locationBut),
        m_editingFile ? m_editingFile->absoPath : "New file");

    this->SetContentWidget(GTK_WIDGET(m_baseGrid));
    gtk_grid_attach(m_baseGrid, GTK_WIDGET(m_baseBox), 0, 2, 1, 1);

    g_object_unref(builder);
}

//private
void EditArea::ConnectSignals(){
    g_signal_connect(m_keyDownEventCtrl, "key-pressed", G_CALLBACK(OnKeyInput), this);
    gtk_widget_add_controller(GTK_WIDGET(m_textView),m_keyDownEventCtrl);
    g_signal_connect(m_focusEventCtrl, "leave", G_CALLBACK(OnUnfocused), this);
    gtk_widget_add_controller(GTK_WIDGET(m_textView), m_focusEventCtrl);
    g_signal_connect(m_mouseMovedEventCtrl, "motion", G_CALLBACK(OnMouseMoved), this);
    gtk_widget_add_controller(GTK_WIDGET(m_textView), m_mouseMovedEventCtrl);
    g_signal_connect(m_textView, "move-cursor", G_CALLBACK(OnCursorMovedByKey),this);
    g_signal_connect(m_textViewBuffer, "insert-text", G_CALLBACK(OnTextInserted), this);
    g_signal_connect(m_textViewBuffer, "delete-range", G_CALLBACK(OnTextDeleted), this);
    g_signal_connect(m_textViewBuffer, "changed", G_CALLBACK(OnTextChanged), this);
    g_signal_connect(m_textViewBuffer, "notify::cursor-position",G_CALLBACK(OnCursorPosChanged),this);
    g_signal_connect(m_saveBut, "clicked", G_CALLBACK(OnSaveButtonClicked), this);
    g_signal_connect(m_langBut, "clicked", G_CALLBACK(OnLangButtonClicked), this);
    g_signal_connect(m_diagnBut, "clicked", G_CALLBACK(OnDiagnosticButtonClicked), this);
}

void EditArea::LoadCursorPos(){
    g_object_get(m_textViewBuffer, "cursor-position", &m_cursorIndex, nullptr);

    gtk_text_buffer_get_iter_at_offset(m_textViewBuffer, &m_cursorItr, m_cursorIndex);
    m_cursorZPos.line = gtk_text_iter_get_line(&m_cursorItr);
    m_cursorZPos.column = gtk_text_iter_get_line_offset(&m_cursorItr);
    std::string pos =
        "Line: "
        + std::to_string(m_cursorZPos.line + 1)
        + '/'
        + std::to_string(m_totalLines)
        + " Column: "
        + std::to_string(m_cursorZPos.column + 1);

    gtk_button_set_label(m_cursorPosBut, pos.c_str());

    /*
     * Set rectangle
     */
    gtk_text_view_get_iter_location(m_textView, &m_cursorItr, &m_cursorRec);
    gtk_text_view_buffer_to_window_coords(m_textView, GTK_TEXT_WINDOW_WIDGET,
        m_cursorRec.x, m_cursorRec.y, &m_cursorRec.x, &m_cursorRec.y);
}

GtkTextBuffer* EditArea::GetTextBuffer(){
    return m_textViewBuffer;
}

GdkRectangle* EditArea::GetCursorRectangle(){
    return &m_cursorRec;
}

const FileData* EditArea::GetFileData() const{
    return m_editingFile;
}

const char* EditArea::GetFilePath() const{
    if(m_editingFile){
        return m_editingFile->absoPath;
    }
    return "";
}

const unsigned int EditArea::GetFileVersion() const{
    return m_fileVersion;
}

const Difference &EditArea::GetPendingDiff() const{
    return m_pendingDif;
}

py_EditArea *EditArea::GetPyEditArea(){
    return m_py_EditArea.get();
}

CompletionTool &EditArea::GetCompletionTool(){
    return *m_completionTool;
}

DiagnosticTool &EditArea::GetDiagnosticTool(){
    return *m_diagnosticTool;
}

void EditArea::SetLanguage(Language* newlang){
    if (newlang == m_language) {
        return;
    }

    Language* oldlang = m_language;
    m_language = newlang;
    gtk_button_set_label(m_langBut, m_language->name);
    syntaxprovider::FastHighlight(this);

    //call callbacks
    const SimpleEvent &event =  m_eventMap.at(EditArea::LANG_CHANGED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LangChangedCallback)callback)(this, oldlang, newlang);
    }
}

void EditArea::LoadFile(FileData* newfile){
    m_editingFile = newfile;
    m_editingFile->editArea = this;

    if(m_editingFile->isVirtual){
        m_originalContent = strdup("\0");
        m_originalLength = 0;
    }else{
        filemanager::ReadFileText(m_editingFile, &m_originalContent);
        m_originalLength = strlen(m_originalContent);
        gtk_text_buffer_set_text(m_textViewBuffer, m_originalContent, m_originalLength);
    }

    gtk_button_set_label(m_locationBut, m_editingFile->absoPath);
    auto lang = langmanager::FindByFileExtension(m_editingFile->fileName);
    this->SetLanguage(lang);
    this->SetContentName(m_editingFile->fileName);

    if(m_parent){
        m_parent->ChildDataChanged(this);
    }

    if(m_child){
        m_child->ParentDataChanged(this);
    }

    SimpleEvent &event = m_eventMap.at(EditArea::FILE_DATA_CHANGED);
    for (EventCallback callback : event.GetCallbackSet()){
        ((EditAreaBasicCallback)callback)(this);
    }
}

void EditArea::Save(){
    gtk_text_buffer_get_start_iter(m_textViewBuffer, &m_startItr);
    gtk_text_buffer_get_end_iter(m_textViewBuffer, &m_endItr);
    const char* content = gtk_text_buffer_get_text(m_textViewBuffer, &m_startItr, &m_endItr, true);

    saving_editarea = this;
    filemanager::SaveFile(m_editingFile, content, OnFileSaved);
}

void EditArea::Close(){
    SimpleEvent &event = m_eventMap.at(EditArea::CLOSED);
    for (EventCallback callback : event.GetCallbackSet()){
        ((EditAreaBasicCallback)callback)(this);
    }

    session::CloseFile(m_editingFile);
}

void EditArea::ShowSearchDialog() {
    if (!searchDialog) {
        searchDialog = std::make_unique<SearchReplaceDialog>(m_textView, m_textViewBuffer);
    }
    searchDialog->Show();
}

void EditArea::ShowReplaceDialog() {
    if (!searchDialog) {
        searchDialog = std::make_unique<SearchReplaceDialog>(m_textView, m_textViewBuffer);
    }
    searchDialog->Show();
}

void EditArea::Insert(unsigned int line, unsigned int column, const char* text){
    gtk_text_iter_set_line(&m_startItr, line);
    gtk_text_iter_set_line_offset(&m_startItr, column);
    gtk_text_buffer_insert(m_textViewBuffer, &m_cursorItr, text, strlen(text));
}

void EditArea::InsertAtCursor(const char* text){
    gtk_text_buffer_insert(m_textViewBuffer, &m_cursorItr, text, strlen(text));
}



/*
 * callbacks
 */

bool EditArea::KeyInput(guint keyval, guint keycode, GdkModifierType state){
    // Search and replace shortcuts
    if (state & GDK_CONTROL_MASK) {
        switch (keyval) {
        case GDK_KEY_f:
            this->ShowSearchDialog();
            return true;
        case GDK_KEY_h:
            this->ShowReplaceDialog();
            return true;
        case GDK_KEY_s:
            this->Save();
            return true;
        case GDK_KEY_o:
            filemanager::ChooseFile();
            return true;
        default:
            break;
        }
    }

    if (state & GDK_ALT_MASK) {
        if (keyval == GDK_KEY_s) {
            //gui::GetSettingPanel()->Show();
        }
    }

    switch (keyval) {
    case GDK_KEY_Tab:
        this->InsertAtCursor("    ");
        return true;
    default:
        break;
    }
    return false;
}

void EditArea::Unfocused(){
    m_diagnosticPopover->Hide();
}

void EditArea::MouseMoved(double x, double y){

    double adjx = gtk_adjustment_get_value(m_hAdjustment);
    double adjy = gtk_adjustment_get_value(m_vAdjustment);

    gtk_text_view_get_iter_at_position(m_textView, &m_startItr, 0, x + adjx, y + adjy);
    ZPosition zpos{
        .line = (unsigned int)gtk_text_iter_get_line(&m_startItr),
        .column = (unsigned int)gtk_text_iter_get_line_index(&m_startItr)
    };// should not be less than 0

    Diagnostic* d = m_diagnosticTool->Find(zpos);
    if(d){
        if (d != m_diagnosticPopover->GetShowingDiagnostic()) {
            GdkRectangle r;
            GtkTextIter itr;
            ZPosition& startpos = d->range.start;
            gtk_text_buffer_get_iter_at_line_offset(m_textViewBuffer, &itr, startpos.line, startpos.column);
            gtk_text_view_get_iter_location(m_textView, &itr, &r);
            m_diagnosticPopover->Show(*d, r.x - adjx, r.y - adjy);
            //printf("x:%f y:%f\n", x - adjx, r.y - adjy);
        }
    }else {
        m_diagnosticPopover->Hide();
    }
}

void EditArea::CursorMovedByKey(){
    m_isCurMovedByKey = true;
}

void EditArea::TextInserted(GtkTextIter* itr, char* text, long int length){
    m_fileVersion += 1;
    this->LoadCursorPos();

    tools::GetZPosFromGtkTextIter(m_pendingDif.before.start, itr);
    tools::GetZPosFromGtkTextIter(m_pendingDif.before.end, itr);
    m_pendingDif.text = text;
}

void EditArea::TextDeleted(GtkTextIter* start, GtkTextIter* end){
    m_fileVersion += 1;
    this->LoadCursorPos();

    tools::GetZPosFromGtkTextIter(m_pendingDif.before.start, start);
    tools::GetZPosFromGtkTextIter(m_pendingDif.before.end, end);
    m_pendingDif.text = strdup("");
}

void EditArea::TextChanged(){
    if(isSaved == true){
        isSaved = false;
        gtk_button_set_label(m_saveBut, "Save");
    }

    m_isCurMovedByKey =true;
    m_isTextChanged = true;

    syntaxprovider::FastHighlight(this);
    SimpleEvent &event = m_eventMap.at(EditArea::TEXT_CHANGED);
    for (EventCallback callback : event.GetCallbackSet()){
        ((EditAreaBasicCallback)callback)(this);
    }

    this->LoadCursorPos();

    if (!gtk_text_iter_starts_line(&m_cursorItr)) {
        GtkTextIter* previtr = gtk_text_iter_copy(&m_cursorItr);
        gtk_text_iter_backward_char(previtr);
        char charbefore = *gtk_text_buffer_get_text(m_textViewBuffer, previtr, &m_cursorItr, false);
        //printf("before %c\n", charbefore);
        if (charbefore != ' ' && charbefore != '\t') {
            SimpleEvent& event = m_eventMap.at(COMPLETION_REQUESTED);
            for (EventCallback callback : event.GetCallbackSet()) {
                ((CompletionRequestedCallback)callback)(this, m_cursorZPos);
            }
        }
    }
}

void EditArea::CursorPosChanged(){
    this->LoadCursorPos();

    SimpleEvent &event = m_eventMap.at(EditArea::CURSOR_MOVED);
    for (EventCallback callback : event.GetCallbackSet()){
        ((EditAreaBasicCallback)callback)(this);
    }

    if(m_isCurMovedByKey == true){
        gtk_text_view_scroll_to_iter(m_textView, &m_cursorItr, 0.4 ,false, 0.4, 0.4);
        m_isCurMovedByKey = false;
    }

    if(m_isTextChanged == false){
        //lsppopovers::suggestion::Hide();
        //lsppopovers::tip::Hide();
    }else{
        m_isTextChanged = false;
    }
};

void EditArea::FileSaved(FileData *filedata){
    if(filedata == nullptr){
        //saving cancelled
        return;
    }
    if(filedata != m_editingFile){
        this->LoadFile(filedata);
    }

    gtk_text_buffer_get_bounds(m_textViewBuffer, &m_startItr, &m_endItr);
    m_originalContent = gtk_text_buffer_get_text(m_textViewBuffer, &m_startItr, &m_endItr, true);

    isSaved = true;
    gtk_button_set_label(m_saveBut, "Saved");

    SimpleEvent &event = m_eventMap.at(EditArea::FILE_SAVED);
    for (EventCallback callback : event.GetCallbackSet()){
        ((EditAreaBasicCallback)callback)(this);
    }
}

void EditArea::DiagnosticButtonClicked(){
    gui::GetDiagnosticPanel()->ShowFor(this);
}

void EditArea::LangButtonClicked(){
    gui::GetLangPanel()->ChooseFor(this);
}


void EditArea::Listen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if(itr != m_eventMap.end()){
        itr->second.Connect(callback);
    }
}

void EditArea::StopListen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if(itr != m_eventMap.end()){
        itr->second.Disconnect(callback);
    }
}
