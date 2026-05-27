#include "EditArea.h"

#include "EditArea_if.h"
#include "SearchReplaceDialog.h"
#include "LangPanel.h"
#include "Popovers.h"
#include "datatypes/common.h"
#include "datatypes/lsp.h"
#include "datatypes/language.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileOperation.h"
#include "src/gui/components/TextArea.h"
#include "src/languages/LanguageManager_if.h"
#include "src/filemanagement/FileManagement_if.h"
#include "toolset/syntaxprovider/syntax_provider.h"
#include "toolset/tools/Tool.h"
#include "pythonbackend/editarea/editarea_mod_Py.h"

#include <cstring>
#include <gdk/gdkkeysyms.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

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
    langpanel::ChooseLanguage(parent);
}

static void OnLangChanged(TextArea *parent, Language* lang){
    EditArea *ea = (EditArea*)parent;
    ea->LangChanged();
    syntaxprovider::FastHighlight(ea);
}

static void OnFileSaved(FileData* file){
    saving_editarea->FileSaved(file);
    saving_editarea = nullptr;
}

EditArea::EditArea(FileData* file){
    editarea::InsertToEditAreaList(this);
    /* Load gui */
    this->LoadGui();
    this->LoadFile(file);
    m_diagnosticPopover = new DiagnosticPopover(m_textView, m_textViewBuffer);

    /* Initialize variables */
    m_cursorIndex = 0;
    m_isCurMovedByKey = false;

    /* Set properties */
    gtk_button_set_label(m_saveBut, "Saved");

    this->LoadCursorPos();
    this->ConnectSignals();

    if(m_language == nullptr){
        SetLanguage(langmanager::FindLanguage("Unknown"));
    }
}

EditArea::~EditArea(){
    delete m_diagnosticPopover;
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
    m_errorBut = GTK_BUTTON(gtk_builder_get_object(builder, "error-btn"));
    m_errorButLabel = GTK_LABEL(gtk_builder_get_object(builder, "error-btn-label"));
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
    g_signal_connect_after(m_textViewBuffer, "changed", G_CALLBACK(OnTextChanged), this);
    g_signal_connect_after(m_textViewBuffer, "notify::cursor-position",G_CALLBACK(OnCursorPosChanged),this);
    g_signal_connect(m_saveBut, "clicked", G_CALLBACK(OnSaveButtonClicked), this);
    g_signal_connect(m_langBut, "clicked", G_CALLBACK(OnLangButtonClicked), this);//Choose language is done by TextTag.cpp

    this->ListenEvent(TEXTAREA_CLASS_LANG_CHANGED,(EventCallback)OnLangChanged);
}

void EditArea::AddDiagnostic(Diagnostic* diagnostic){
    m_diagnosticsList.emplace(diagnostic);
}

void EditArea::ProcessDiagnostics(){
    char severityList[5] = {-1, 0, 0, 0, 0};
    // [0      , 1    , 2      , 3          , 4   ]
    // [Unknown, Error, Warning, Information, Hint]
    static const char* tags[5] = {
        "none",// severity must not be 0
        "error",
        "warning",
        "info",
        "hint"
    };

    for (Diagnostic* diagnostic: m_diagnosticsList) {
        this->ApplyTagByRange(&diagnostic->range, tags[diagnostic->severity]);
        severityList[diagnostic->severity] ++;
    }

    std::string s =
        "<span color=\"red\">e" +
        std::to_string(severityList[1]) +
        "</span> <span color=\"yellow\">w" +
        std::to_string(severityList[2]) +
        "</span> <span color=\"greenyellow\">i" +
        std::to_string(severityList[3]) +
        "</span>";
    gtk_label_set_markup(m_errorButLabel, s.c_str());
}

void EditArea::ClearDiagnostics(){
    for (Diagnostic* diagnostic : m_diagnosticsList){
        delete diagnostic;
    }
    m_currentDiagnosticRange.start.line = 0;
    m_currentDiagnosticRange.start.column = 0;
    m_currentDiagnosticRange.end.line = 0;
    m_currentDiagnosticRange.end.column = 0;
    m_diagnosticsList.clear();
    this->ProcessDiagnostics();
}

Diagnostic* EditArea::FindDiagnostic(GtkTextIter* itr){
    ZPosition pos;
    pos.line = gtk_text_iter_get_line(&m_startItr);
    pos.column = gtk_text_iter_get_line_index(&m_startItr);
    // printf("line %i index %i\n", line, index);
    if(tools::IsZPosInRange(pos, &m_currentDiagnosticRange)) return nullptr;

    // There may be more than 1 diagnostic for a iteer
    for (Diagnostic* diagnostic : m_diagnosticsList) {
        if(tools::IsZPosInRange(pos, &diagnostic->range)){
            m_currentDiagnosticRange = diagnostic->range;
            return diagnostic;
        }
    }
    m_currentDiagnosticRange.start.line = 0;
    m_currentDiagnosticRange.start.column = 0;
    m_currentDiagnosticRange.end.line = 0;
    m_currentDiagnosticRange.end.column = 0;
    m_diagnosticPopover->Hide();
    return nullptr;
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

void EditArea::SetLanguage(Language* lang){
    if (lang == m_language) {
        return;
    }

    m_language = lang;
    gtk_button_set_label(m_langBut, m_language->name.c_str());
    syntaxprovider::FastHighlight(this);
    //call callbacks
    for (auto callback : m_langChangedCallbacks) {
        callback(this, lang);
    }
}

void EditArea::LoadFile(FileData* newfile){
    editarea::RemoveFromEditAreaList(this);
    m_editingFile = newfile;
    if(m_editingFile->isVirtual){
        m_originalContent = strdup("\0");
        m_originalLength = 0;
    }else{
        filemanagement::ReadFileText(m_editingFile, &m_originalContent);
        m_originalLength = strlen(m_originalContent);

        gtk_text_buffer_set_text(m_textViewBuffer, m_originalContent, -1);
    }

    editarea::InsertToEditAreaList(this);

    gtk_button_set_label(m_locationBut, m_editingFile->absoPath);

    this->SetLanguage(langmanager::FindFileLanguage(m_editingFile->fileName));
    this->SetContentName(m_editingFile->fileName);
    if(m_parent){
        m_parent->ChildDataChanged(this);
    }
    if(m_child){
        m_child->ParentDataChanged(this);
    }

    editarea_py_invoke_filedata_changed(this);
}



void EditArea::Destroy(){
    editarea::CloseFile(m_editingFile);
}

void EditArea::Save(){
    gtk_text_buffer_get_start_iter(m_textViewBuffer, &m_startItr);
    gtk_text_buffer_get_end_iter(m_textViewBuffer, &m_endItr);
    char* content = gtk_text_buffer_get_text(m_textViewBuffer, &m_startItr, &m_endItr, true);

    saving_editarea = this;
    filemanagement::SaveFile(m_editingFile, content, OnFileSaved);
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
            filemanagement::ChooseFile();
            return true;
        default:
            break;
        }
    }

    if (state & GDK_ALT_MASK) {
        if (keyval == GDK_KEY_s) {
            //gui::AppSettingPanel.Show();
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
    int bufferx, buffery;

    gtk_text_view_window_to_buffer_coords(m_textView, GTK_TEXT_WINDOW_WIDGET, x, y, &bufferx, &buffery);
    gtk_text_view_get_iter_at_position(m_textView, &m_startItr, 0, x, y);
    int textviewline = gtk_text_iter_get_line(&m_startItr);
    int textviewindex = gtk_text_iter_get_line_index(&m_startItr);
    gtk_text_view_get_iter_at_position(m_textView, &m_startItr, 0, bufferx, buffery);
    Diagnostic* d = this->FindDiagnostic(&m_startItr);
    if(d){
        m_diagnosticPopover->ShowDiagnostic(d, textviewline, textviewindex);
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
    editarea_py_invoke_text_changed(this);
}

void EditArea::LangChanged(){
    editarea_py_invoke_lang_changed(this);
}

void EditArea::CursorPosChanged(){
    this->LoadCursorPos();
    editarea_py_invoke_cursor_moved(this, m_cursorZPos);
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

void EditArea::FileSaved(FileData *file){
    if(file == nullptr){
        //saving cancelled
        return;
    }
    if(file != m_editingFile){
        this->LoadFile(file);
    }else{
        gtk_text_buffer_get_bounds(m_textViewBuffer, &m_startItr, &m_endItr);
        m_originalContent = gtk_text_buffer_get_text(m_textViewBuffer, &m_startItr, &m_endItr, true);
    }

    isSaved = true;
    gtk_button_set_label(m_saveBut, "Saved");

    editarea_py_invoke_file_saved(this);

}

/*
 * Private
 */
