#include "EditArea.h"

#include "EditArea_if.h"
#include "datatypes/language.h"
#include "toolset/syntaxprovider/syntax_provider.h"
#include "toolset/tools/text_tool.h"
#include "src/filemanagement/FileManagement_if.h"
#include "pythonbackend/editarea/editarea_mod_Py.h"
#include "src/languages/LanguageManager_if.h"
#include "SearchReplaceDialog.h"
#include "LangPanel.h"
#include "LspPopovers_if.h"

#include <glib.h>

/*
 * EditArea class
 */

static EditArea *saving_editarea;// cache the edit area that is waiting for saving

static void Unfocused(GtkEventControllerFocus* self, EditArea* parent){
    parent->UnfocusedCallback();
}

static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *parent){
    parent->CursorMovedByKeyCallback();
}

static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* parent){
    parent->TextChangedCallback();
}

static bool KeyInput(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, EditArea* parent){
    return parent->KeyInputCallback(keyval, keycode, state);;
}

static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *parent){
    parent->CursorPosChangedCallback();
}

static void SaveButtonClicked(GtkButton *self, EditArea* parent){
    parent->Save();
}

static void LangButtonClicked(GtkButton *self, EditArea *parent){
    langpanel::ChooseLanguage(parent);
}

static void LangChangedCallback(TextArea *parent, datatypes::Language* lang){
    EditArea *ea = (EditArea*)parent;
    syntaxprovider::FastHighlight(ea);
}

static void FileSaved(GFile* file){
    saving_editarea->FileSavedCallback(file);
    saving_editarea = nullptr;
}

EditArea::EditArea(GFile* file){
    /* Load gui */
    this->LoadGui();
    this->LoadFile(file);
    /* Initialize variables */

    m_totalLines = 1;
    m_cursorPos = 0;
    m_isCurMovedByKey = false;

    /* Set properties */
    gtk_button_set_label(m_saveBut, "Saved");

    this->CountError();
    this->LoadCursorPos();
    this->ConnectSignals();

    if(m_language == nullptr){
        setLanguage(langmanager::FindLanguage("Unknown"));
    }
}

EditArea::~EditArea(){
}

//private
void EditArea::LoadGui(){
    /* Loading EditArea from UI/EditArea.ui */
    GtkBuilder *builder = gtk_builder_new_from_file("ui/EditArea.ui");

    /* Binding */
    m_baseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    // FileInfo panel(top)
    m_locationBut = GTK_BUTTON(gtk_builder_get_object(builder,"LocationBut"));
    m_saveBut = GTK_BUTTON(gtk_builder_get_object(builder, "SaveBut"));
    // Misc panel(bottom)
    m_outlineBut = GTK_BUTTON(gtk_builder_get_object(builder, "OutlineBut"));
    m_errorBut = GTK_BUTTON(gtk_builder_get_object(builder, "ErrorBut"));
    m_errorButLabel = GTK_LABEL(gtk_builder_get_object(builder, "ErrorButLabel"));
    m_langBut = GTK_BUTTON(gtk_builder_get_object(builder, "LangBut"));
    m_cursorPosBut = GTK_BUTTON(gtk_builder_get_object(builder, "CursorPosBut"));

    m_keyDownEventCtrl = gtk_event_controller_key_new();
    m_focusEventCtrl = gtk_event_controller_focus_new();

    gtk_widget_set_has_tooltip(GTK_WIDGET(m_saveBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(m_saveBut), "Save");
    gtk_widget_set_has_tooltip(GTK_WIDGET(m_locationBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(m_locationBut),
        m_editingFile ? g_file_get_path(m_editingFile) : "New file");

    this->setContentWidget(GTK_WIDGET(m_baseGrid));
    gtk_grid_attach(m_baseGrid, GTK_WIDGET(m_baseBox), 0, 2, 1, 1);

    g_object_unref(builder);
}

//private
void EditArea::ConnectSignals(){
    g_signal_connect(m_keyDownEventCtrl, "key-pressed", G_CALLBACK(KeyInput), this);
    gtk_widget_add_controller(GTK_WIDGET(m_textView), GTK_EVENT_CONTROLLER(m_keyDownEventCtrl));
    g_signal_connect(m_focusEventCtrl, "leave", G_CALLBACK(Unfocused), this);
    gtk_widget_add_controller(GTK_WIDGET(m_textView), m_focusEventCtrl);
    g_signal_connect(m_textView, "move-cursor", G_CALLBACK(CursorMovedByKey),this);
    g_signal_connect_after(m_textViewBuffer, "notify::text",G_CALLBACK(TextChanged),this);
    g_signal_connect_after(m_textViewBuffer, "notify::cursor-position",G_CALLBACK(CursorPosChanged),this);
    g_signal_connect(m_saveBut, "clicked", G_CALLBACK(SaveButtonClicked), this);
    g_signal_connect(m_langBut, "clicked", G_CALLBACK(LangButtonClicked), this);//Choose language is done by TextTag.cpp

    AddLangChangedCallback(LangChangedCallback);
}

void EditArea::CountError(){
    int err = 0;
    int warn = 0;
    int info = 0;
    std::string s = "<span color=\"red\">⚠" + std::to_string(err) + "</span> <span color=\"yellow\">⚠" + std::to_string(warn)+ "</span> <span color=\"greenyellow\">⚠"+ std::to_string(info)+"</span>";
    gtk_label_set_markup(m_errorButLabel, s.c_str());
}

void EditArea::LoadCursorPos(){
    g_object_get(m_textViewBuffer, "cursor-position", &m_cursorPos, nullptr);

    gtk_text_buffer_get_iter_at_offset(m_textViewBuffer, &m_cursorItr, m_cursorPos);
    m_cursorLine = gtk_text_iter_get_line(&m_cursorItr) + 1;
    m_cursorColumn = gtk_text_iter_get_line_offset(&m_cursorItr) + 1;
    std::string Pos =
        "Line: "
        + std::to_string(m_cursorLine)
        + '/'
        + std::to_string(m_totalLines)
        + " Column: "
        + std::to_string(m_cursorColumn);
    gtk_button_set_label(m_cursorPosBut, Pos.c_str());

    /*
     * Set rectangle
     */
    gtk_text_view_get_iter_location(m_textView, &m_cursorItr, &m_cursorRec);
    gtk_text_view_buffer_to_window_coords(m_textView, GTK_TEXT_WINDOW_WIDGET,
        m_cursorRec.x, m_cursorRec.y, &m_cursorRec.x, &m_cursorRec.y);
}

GtkTextBuffer* EditArea::getTextBuffer(){
    return m_textViewBuffer;
}

GdkRectangle* EditArea::getCursorRectangle(){
    return &m_cursorRec;
}

const char* EditArea::getFilePath(){
    return m_absoPath.c_str();
}

void EditArea::setLanguage(datatypes::Language* lang){
    m_language = lang;
    gtk_button_set_label(m_langBut, m_language->name.c_str());
    //call callbacks
    for (auto callback : m_langChangedCallbacks) {
        callback(this, lang);
    }
}

void EditArea::LoadFile(GFile* newfile){
    m_editingFile = newfile;
    if(m_editingFile == nullptr){
        m_fileName = "untitled";
        m_absoPath = tools::GenerateId().c_str();
    }else{
        m_fileName = g_file_get_basename(m_editingFile);
        m_absoPath = g_file_get_path(m_editingFile);

        char *content;
        filemanagement::ReadFileText(m_editingFile, &content);

        gtk_button_set_label(m_locationBut, m_fileName.c_str());
        gtk_text_buffer_set_text(m_textViewBuffer, content, -1);
        g_free(content);
    }

    gtk_button_set_label(m_locationBut, m_absoPath.c_str());
    this->setContentName(m_fileName);
    editarea_py_invoke_filedata_changed(this);

    if(m_parent){
        m_parent->ChildDataChanged(this);
    }
    if(m_child){
        m_child->ParentDataChanged(this);
    }
}



void EditArea::Destroy(){
    editarea::CloseFile(m_editingFile);
}

void EditArea::Save(){
    gtk_text_buffer_get_start_iter(m_textViewBuffer, &m_startItr);
    gtk_text_buffer_get_end_iter(m_textViewBuffer, &m_endItr);
    char* content = gtk_text_buffer_get_text(m_textViewBuffer, &m_startItr, &m_endItr, true);

    saving_editarea = this;
    filemanagement::SaveFile(m_editingFile, content, FileSaved);
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


//callbacks
void EditArea::UnfocusedCallback(){
    lsppopovers::suggestion::Hide();
    lsppopovers::tip::Hide();
}

void EditArea::CursorMovedByKeyCallback(){
    m_isCurMovedByKey = true;
}

void EditArea::TextChangedCallback(){
    if(m_pauseTextChangedCallback){
        return;
    }

    if(m_isSaved == true){
        m_isSaved = false;
        gtk_button_set_label(m_saveBut, "Save");
    }

    this->LoadCursorPos();
    m_isCurMovedByKey =true;
    m_isTextChanged = true;

    lsppopovers::suggestion::Hide();
    syntaxprovider::FastHighlight(this);

    //editarea_py_invoke_text_changed(this);

    //see if there is any text before the text iter.
    //if yes then trigger autocompletion
    m_startItr = *gtk_text_iter_copy(&m_cursorItr);
    gtk_text_iter_backward_char(&m_startItr);
    char previouschar = gtk_text_iter_get_text(&m_startItr, &m_cursorItr)[0];

    if(previouschar == ' ' || previouschar == '\t' || previouschar == '\n' || previouschar == '\r' || previouschar == 0){
        // don't trigger auto completion if there is a space before the cursor
    }else{
        editarea_py_invoke_completion_requested(this);
    }
}
bool EditArea::KeyInputCallback(guint keyval, guint keycode, GdkModifierType state){
    // Search and replace shortcuts
    if (state & GDK_CONTROL_MASK) {
        if (keyval == GDK_KEY_f) {
            this->ShowSearchDialog();
            return true;
        } else if (keyval == GDK_KEY_h) {
            this->ShowReplaceDialog();
            return true;
        } else if (keyval == GDK_KEY_s){
            this->Save();
            return true;
        } else if (keyval == GDK_KEY_o){
            //filemanager::ChooseFile();
        } else if(keyval == GDK_KEY_t){
            //
        }
    }

    if (state & GDK_ALT_MASK) {
        if (keyval == GDK_KEY_s) {
            //gui::AppSettingPanel.Show();
        }
    }
/*
    else if(m_sugPopover->m_isShowing){
        if(keyval == GDK_KEY_Up){
            m_sugPopover->SelectUp();
            return true;
        }else if(keyval == GDK_KEY_Down){
            m_sugPopover->SelectDown();
            return true;
        }else if(keyval == GDK_KEY_Tab || keyval  == GDK_KEY_Return){
            // We have to stop autocompletion for blank space
            // before using Enter key to select the completion.
            // Else whenever the user spams Enter to create new lines,
            // the completion will be inserted.
            m_sugPopover->ConfirmSelection();
            return true;
        }
    }
*/
    return false;
}
void EditArea::CursorPosChangedCallback(){
    this->LoadCursorPos();
    editarea_py_invoke_cursor_moved(this, m_cursorLine, m_cursorColumn);
    if(m_isCurMovedByKey == true){
        gtk_text_view_scroll_to_iter(m_textView, &m_cursorItr, 0.4 ,false, 0.4, 0.4);
        m_isCurMovedByKey = false;
    }

    if(m_isTextChanged == false){
        lsppopovers::suggestion::Hide();
        lsppopovers::tip::Hide();
    }else{
        m_isTextChanged = false;
    }
};

void EditArea::FileSavedCallback(GFile *file){
    if(file == nullptr){
        //saving cancelled
        return;
    }
    if(file != m_editingFile){
        this->LoadFile(file);
    }

    m_isSaved = true;
    gtk_button_set_label(m_saveBut, "Saved");

    editarea_py_invoke_file_saved(this);
    //gtk_widget_remove_css_class(GTK_WIDGET(EditAreacache->m_parentSwitcher->m_Button), "SwitcherButtonUnsaved");
    //gtk_widget_add_css_class(GTK_WIDGET(EditAreacache->m_parentSwitcher->m_Button), "SwitcherButtonSaved");
}

/*
 * Private
 */
