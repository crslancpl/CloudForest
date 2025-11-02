#include "EditArea.h"

#include <cerrno>
#include <cstddef>
#include <cstring>
#include <gdk/gdkkeysyms.h>
#include <gio/gmenu.h>
#include <glib/gprintf.h>
#include <glib.h>
#include <glibconfig.h>

#include <gtk/gtk.h>
#include <iterator>
#include <memory>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <type_traits>
#include <utility>

#include "../ToolFunctions.h"
#include "../cf/CFEmbed.h"
#include "SettingPanel.h"
#include "Style.h"
#include "EditArea.h"
#include "FilePanel.h"
#include "MainWindow.h"
#include "guiCore.h"
#include "SearchReplaceDialog.h"
#include "CFPopover.h"

/*
 * EditArea class
 */

static void unfocused(GtkEventControllerFocus* self, EditArea* parent){
    parent->m_Tippopover->Hide();
    parent->m_Sugpopover->Hide();
}


static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent){
    Parent->m_IsCurMovedByKey = true;
}

static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent){
    if(Parent->m_PauseTextChangedCallback){
        return;
    }

    if(Parent->m_IsSaved == true){
        Parent->m_IsSaved = false;
        gtk_widget_remove_css_class(GTK_WIDGET(Parent->m_ParentSwitcher->m_Button), "SwitcherButtonSaved");
        gtk_widget_add_css_class(GTK_WIDGET(Parent->m_ParentSwitcher->m_Button), "SwitcherButtonUnsaved");
        gtk_button_set_label(Parent->m_SaveBut, "Save");
    }

    Parent->LoadCursorPos();
    Parent->m_IsCurMovedByKey =true;
    Parent->m_IsTextChanged = true;
    Parent->CountLine();
    Parent->HighlightSyntax();
    Parent->HideSuggestion();


    //run the callbacks for python
    gui::PythonCallbackEATextChanged(Parent);

    //see if there is any text before the text iter.
    //if yes then trigger autocompletion
    Parent->StartItr = gtk_text_iter_copy(Parent->CursorItr);
    gtk_text_iter_backward_char(Parent->StartItr);
    char previouschar = gtk_text_iter_get_text(Parent->StartItr, Parent->CursorItr)[0];

    if(previouschar == ' ' || previouschar == '\t' || previouschar == '\n' || previouschar == '\r' || previouschar == 0){
        // don't trigger auto completion if there is a space before the cursor
    }else{
        gui::PythonCallbackEAReqCompletion(Parent);
    }

}

static bool KeyInput(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, EditArea* Parent){
    // Search and replace shortcuts
    if (state & GDK_CONTROL_MASK) {
        if (keyval == GDK_KEY_f) {
            Parent->ShowSearchDialog();
            return true;
        } else if (keyval == GDK_KEY_h) {
            Parent->ShowReplaceDialog();
            return true;
        } else if (keyval == GDK_KEY_s){
            Parent->Save();
            return true;
        } else if (keyval == GDK_KEY_o){
            gui::OpenFileChooser(true);
        } else if(keyval == GDK_KEY_t){
            //
        }
    }

    if (state & GDK_ALT_MASK) {
        if (keyval == GDK_KEY_s) {
            gui::AppSettingPanel.Show();
        }
    } else if(Parent->m_Sugpopover->m_IsShowing){
        if(keyval == GDK_KEY_Up){
            Parent->m_Sugpopover->SelectUp();
            return true;
        }else if(keyval == GDK_KEY_Down){
            Parent->m_Sugpopover->SelectDown();
            return true;
        }else if(keyval == GDK_KEY_Tab || keyval  == GDK_KEY_Return){
            // We have to stop autocompletion for blank space
            // before using Enter key to select the completion.
            // Else whenever the user spams Enter to create new lines,
            // the completion will be inserted.
            Parent->m_Sugpopover->ConfirmSelection();
            return true;
        }
    }

    if(keyval == GDK_KEY_Tab){
        gtk_text_buffer_insert_at_cursor(Parent->m_TextViewBuffer, "    ", 4);
        return true;
    }

    return false;
}

static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent){
    Parent->LoadCursorPos();
    if(Parent->m_IsCurMovedByKey == true){
        gtk_text_view_scroll_to_iter(Parent->m_TextView, Parent->CursorItr, 0.4 ,false, 0.4, 0.4);
        Parent->m_IsCurMovedByKey = false;
    }

    if(Parent->m_IsTextChanged == false){
        Parent->m_Tippopover->Hide();
        Parent->m_Sugpopover->Hide();
    }else{
        Parent->m_IsTextChanged = false;
    }
}

static void SaveButtonClicked(GtkButton *self, EditArea* parent){
    parent->Save();
}


EditArea::EditArea(GFile *file){
    /* Load gui */
    LoadGui();

    /* Initialize variables */
    m_EditingFile = file;
    m_cacheTotalLine = 0;
    m_CursorPos = 0;
    m_IsCurMovedByKey = false;

    /* Set properties */
    gtk_button_set_label(m_SaveBut, "Saved");
    gtk_scrollable_set_vadjustment(GTK_SCROLLABLE(m_LineNoArea), gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(m_TextView)));
    gtk_text_view_set_bottom_margin (m_TextView, 80);
    gtk_text_view_set_bottom_margin (m_LineNoArea, 80);
    gtk_text_view_set_pixels_below_lines(m_TextView, 5);
    gtk_text_view_set_pixels_below_lines(m_LineNoArea, 5);

    /* Load file and other infos*/
    LoadFile(file);
    CountLine();
    CountError();
    LoadCursorPos();


    /* Connect signals */
    ConnectSignals();

    style::LoadTextTag(m_TextViewBuffer);
    gui::PythonRegisterEA(this);
    if(m_Language.empty()){
        ChangeLanguage("demo", false);
    }
}

EditArea::~EditArea(){
    delete [] CursorItr;
    delete [] StartItr;
    delete [] EndItr;
}

//private
void EditArea::LoadGui(){
    /* Loading EditArea from UI/EditArea.ui */
    m_Builder = gtk_builder_new_from_file("UI/EditArea.ui");

    /* Binding */
    m_BaseGrid = GTK_GRID(gtk_builder_get_object(m_Builder, "BaseGrid"));
    // FileInfo panel(top)
    m_LocationBut = GTK_BUTTON(gtk_builder_get_object(m_Builder,"LocationBut"));
    m_SaveBut = GTK_BUTTON(gtk_builder_get_object(m_Builder, "SaveBut"));
    // Misc panel(bottom)
    m_OutlineBut = GTK_BUTTON(gtk_builder_get_object(m_Builder, "OutlineBut"));
    m_ErrorBut = GTK_BUTTON(gtk_builder_get_object(m_Builder, "ErrorBut"));
    m_ErrorButLabel = GTK_LABEL(gtk_builder_get_object(m_Builder, "ErrorButLabel"));
    m_LangBut = GTK_BUTTON(gtk_builder_get_object(m_Builder, "LangBut"));
    m_CursorPosBut = GTK_BUTTON(gtk_builder_get_object(m_Builder, "CursorPosBut"));
    // TextView
    m_TextView = GTK_TEXT_VIEW(gtk_builder_get_object(m_Builder, "TextArea"));
    m_TextViewBuffer = gtk_text_view_get_buffer(m_TextView);
    m_LineNoArea = GTK_TEXT_VIEW(gtk_builder_get_object(m_Builder, "LineNum"));
    m_LineNoAreaBuffer = gtk_text_view_get_buffer(m_LineNoArea);
    m_KeyDownEventCtrl = gtk_event_controller_key_new();
    m_FocusEventCtrl = gtk_event_controller_focus_new();

    m_CursorRec = new GdkRectangle();
    m_Tippopover = new TipPopover();
    m_Tippopover->Init(GTK_WIDGET(m_TextView));
    m_Sugpopover = new SuggestionPopover();
    m_Sugpopover->Init(this);

    CursorItr = new GtkTextIter();
    StartItr = new GtkTextIter();
    EndItr = new GtkTextIter();

    gtk_text_view_set_accepts_tab(m_TextView, false);

    gtk_widget_set_has_tooltip(GTK_WIDGET(m_SaveBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(m_SaveBut), "Save");

    gtk_widget_set_has_tooltip(GTK_WIDGET(m_LocationBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(m_LocationBut),
        m_EditingFile ? g_file_get_path(m_EditingFile) : "New file");
}

//private
void EditArea::ConnectSignals(){
    g_signal_connect(m_KeyDownEventCtrl, "key-pressed", G_CALLBACK(KeyInput), this);
    gtk_widget_add_controller(GTK_WIDGET(m_TextView), GTK_EVENT_CONTROLLER(m_KeyDownEventCtrl));
    g_signal_connect(m_FocusEventCtrl, "leave", G_CALLBACK(unfocused), this);
    gtk_widget_add_controller(GTK_WIDGET(m_TextView), m_FocusEventCtrl);
    g_signal_connect(m_TextView, "move-cursor", G_CALLBACK(CursorMovedByKey),this);
    g_signal_connect_after(m_TextViewBuffer, "notify::text",G_CALLBACK(TextChanged),this);
    g_signal_connect_after(m_TextViewBuffer, "notify::cursor-position",G_CALLBACK(CursorPosChanged),this);
    g_signal_connect(m_SaveBut, "clicked", G_CALLBACK(SaveButtonClicked), this);
    g_signal_connect(m_LangBut, "clicked", G_CALLBACK(style::OpenLangChooser), this);//Choose language is done by TextTag.cpp
}

void EditArea::UnrefBuilder(){
    g_object_unref(m_Builder);
}

void EditArea::CountLine(){
    int NewLineCount = gtk_text_buffer_get_line_count(m_TextViewBuffer);
    if(NewLineCount != m_cacheTotalLine){

        gtk_text_buffer_get_end_iter(m_LineNoAreaBuffer, StartItr);
        if(NewLineCount > m_cacheTotalLine){
            while(m_cacheTotalLine < NewLineCount){
                string Line = to_string(m_cacheTotalLine + 1) + '\n';
                gtk_text_buffer_insert(m_LineNoAreaBuffer, StartItr, Line.c_str(), -1);
                m_cacheTotalLine ++;
            }
        }else{
            gtk_text_buffer_get_iter_at_line(m_LineNoAreaBuffer, EndItr, NewLineCount);
            gtk_text_buffer_delete(m_LineNoAreaBuffer, EndItr, StartItr);
            m_cacheTotalLine = NewLineCount;
        }
    }
}

void EditArea::CountError(){
    int err = 0;
    int warn = 0;
    int info = 0;
    string s = "<span color=\"red\">⚠" + to_string(err) + "</span> <span color=\"yellow\">⚠" + to_string(warn)+ "</span> <span color=\"greenyellow\">⚠"+ to_string(info)+"</span>";
    gtk_label_set_markup(m_ErrorButLabel, s.c_str());
}

void EditArea::LoadCursorPos(){
    g_object_get(m_TextViewBuffer, "cursor-position", &m_CursorPos, nullptr);

    gtk_text_buffer_get_iter_at_offset(m_TextViewBuffer, CursorItr, m_CursorPos);
    m_CursorLine = gtk_text_iter_get_line(CursorItr) + 1;
    m_CursorLinePos = gtk_text_iter_get_line_offset(CursorItr) + 1;
    string Pos = "Line: " + to_string(m_CursorLine) + '/' + to_string(m_cacheTotalLine) + " Offset: " + to_string(m_CursorLinePos);
    gtk_button_set_label(m_CursorPosBut, Pos.c_str());

    /*
     * Set rectangle
     */

    gtk_text_view_get_iter_location(m_TextView, CursorItr, m_CursorRec);
    gtk_text_view_buffer_to_window_coords(m_TextView, GTK_TEXT_WINDOW_WIDGET,
        m_CursorRec->x, m_CursorRec->y, &m_CursorRec->x, &m_CursorRec->y);
}

const std::string& EditArea::GetContent(){
    // To prevent the text to be discarded before the caller
    // get its content, we mark it static so it will not be discarded
    static std::string content;

    gtk_text_buffer_get_start_iter(m_TextViewBuffer, StartItr);
    gtk_text_buffer_get_end_iter(m_TextViewBuffer, EndItr);
    content = gtk_text_buffer_get_text(m_TextViewBuffer, StartItr, EndItr, true);
    return content;
}

void EditArea::ShowSuggestion(){
    m_Sugpopover->Show(m_CursorRec);
}

void EditArea::ClearSuggestion(){
    m_Sugpopover->Clear();
}

void EditArea::AddSuggestion(Suggestion *sug){
    m_Sugpopover->Add(sug);
}

void EditArea::HideSuggestion(){
    m_Sugpopover->Hide();
}

void EditArea::ChangeLanguage(const string &lang, bool highlight){
    gui::cfLoadLanguage(lang);
    m_Language = lang;

    cf_Send_Message(cf_MessageType::RELOAD, nullptr);
    if(highlight){
        HighlightSyntax();
    }
    gtk_button_set_label(m_LangBut, lang.c_str());

}

void EditArea::LoadFile(GFile* newfile){
    if (newfile == nullptr) {
        // Don't open file
        gtk_button_set_label(m_LocationBut, "New File");
        m_FileName = strdup("New File");
        GenerateId(m_AbsoPath);
    }else{
        char *content;
        m_FileName = g_file_get_basename(newfile);
        m_AbsoPath = g_file_get_path(newfile);
        gtk_button_set_label(m_LocationBut, m_FileName.c_str());
        g_file_load_contents(newfile,nullptr,&content, nullptr, nullptr,nullptr);
        gtk_text_buffer_set_text(m_TextViewBuffer, content, -1);
        g_free(content);
        content = nullptr;
        m_EditingFile = newfile;
        if(m_ParentSwitcher != nullptr){
            gtk_button_set_label(m_ParentSwitcher->m_Button, m_FileName.c_str());
        }
    }
}

void EditArea::HighlightSyntax(){
    gtk_text_buffer_get_bounds(m_TextViewBuffer, StartItr, EndItr);
    gtk_text_buffer_remove_all_tags(m_TextViewBuffer, StartItr, EndItr);

    gui::cfProcessFile(m_AbsoPath, m_Language);
}

void EditArea::ApplyTagByLength(unsigned int TextStartPos, unsigned int TextLength, char *TagName){
    /*
     * Position 1 is the position of the first character in the buffer
     */
    gtk_text_buffer_get_iter_at_offset(m_TextViewBuffer, StartItr, TextStartPos);
    gtk_text_buffer_get_iter_at_offset(m_TextViewBuffer, StartItr, TextStartPos + TextLength);
    gtk_text_buffer_apply_tag_by_name(m_TextViewBuffer, TagName, StartItr, EndItr);
}

void EditArea::ApplyTagByPos(unsigned int TextStartPos, unsigned int TextEndPos, char *TagName){
    /*
     * Position 1 is the position of the first character in the buffer
     */
    TextStartPos --;
    gtk_text_buffer_get_iter_at_offset(m_TextViewBuffer, StartItr, TextStartPos);
    gtk_text_buffer_get_iter_at_offset(m_TextViewBuffer, EndItr, TextEndPos);
    gtk_text_buffer_apply_tag_by_name(m_TextViewBuffer, TagName, StartItr, EndItr);
}

void EditArea::ApplyTagByLinePos(unsigned int line, unsigned int pos, unsigned int length,char *TagName){
    /*
     * Line 1 is the first line, and position 1 is the position
     * of the first character in the line So line and position shouldn't
     * be less then 1.
     */

    line--;// change the lines that starts from 1 to 0
    if (pos <= 0 ||strcmp(TagName, "none") == 0) {
        return;
    }
    //g_print("line %i pos %i tag %s\n", line, pos, TagName);
    gtk_text_buffer_get_iter_at_line_offset(m_TextViewBuffer, StartItr, line, pos -1 );
    gtk_text_iter_assign(EndItr, StartItr);
    gtk_text_iter_forward_chars(EndItr, length);
    gtk_text_buffer_apply_tag_by_name(m_TextViewBuffer, TagName, StartItr, EndItr);
}

void EditArea::Destroy(){
    m_ParentHolder->Remove(this);
}

static EditArea *EditAreacache;// cache the edit area that is waiting for saving
static void FileSaved(GFile* file, GFileInfo* info){
    //callback for gui::SaveFile()
    if(file == nullptr){
        //saving cancelled
        EditAreacache = nullptr;
        return;
    }

    EditAreacache->LoadFile(file);
    EditAreacache->m_IsSaved = true;
    gtk_button_set_label(EditAreacache->m_SaveBut, "Saved");
    gtk_widget_remove_css_class(GTK_WIDGET(EditAreacache->m_ParentSwitcher->m_Button), "SwitcherButtonUnsaved");
    gtk_widget_add_css_class(GTK_WIDGET(EditAreacache->m_ParentSwitcher->m_Button), "SwitcherButtonSaved");
    EditAreacache = nullptr;
}

void EditArea::Save(){
    gtk_text_buffer_get_start_iter(m_TextViewBuffer, StartItr);
    gtk_text_buffer_get_end_iter(m_TextViewBuffer, EndItr);
    char* content = gtk_text_buffer_get_text(m_TextViewBuffer, StartItr, EndItr, true);

    EditAreacache = this;
    //async
    gui::SaveFile(m_EditingFile, content, FileSaved);
}

void EditArea::ShowSearchDialog() {
    if (!searchDialog) {
        searchDialog = std::make_unique<SearchReplaceDialog>(this);
    }
    searchDialog->Show();
}

void EditArea::ShowReplaceDialog() {
    if (!searchDialog) {
        searchDialog = std::make_unique<SearchReplaceDialog>(this);
    }
    searchDialog->Show();
}





/*
 * EditAreaHolderTabBut class
 */

static void SwitcherButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent){
    if (Parent->m_CorrespondingEA != nullptr) {
        Parent->m_ParentHolder->Show(Parent->m_CorrespondingEA);
    }
}

static void SwitcherCloseButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent){
    gui::RemoveEditArea(Parent->m_CorrespondingEA.get());
}

void EditAreaHolderTabBut::Init(const shared_ptr<EditArea> &editarea, EditAreaHolder& parentholder){
    m_CorrespondingEA = editarea;
    m_ParentHolder = &parentholder;
    m_BaseBox = GTK_BOX(gtk_box_new(GtkOrientation::GTK_ORIENTATION_HORIZONTAL, 0));
    m_Button = GTK_BUTTON(gtk_button_new_with_label(editarea->m_FileName.c_str()));
    m_CloseButton = GTK_BUTTON(gtk_button_new_with_label("⛌"));
    gtk_widget_add_css_class(GTK_WIDGET(m_BaseBox), string("SwitcherBaseBox").c_str());
    gtk_widget_add_css_class(GTK_WIDGET(m_Button), string("SwitcherButtonSaved").c_str());
    gtk_widget_add_css_class(GTK_WIDGET(m_CloseButton), string("SwitcherCloseButton").c_str());
    gtk_box_append(m_BaseBox, GTK_WIDGET(m_Button));
    gtk_box_append(m_BaseBox, GTK_WIDGET(m_CloseButton));

    editarea->m_ParentSwitcher = this;
    editarea->m_ParentHolder = &parentholder;

    g_signal_connect(m_Button, "clicked", G_CALLBACK(SwitcherButtonClicked),this);
    g_signal_connect(m_CloseButton, "clicked", G_CALLBACK(SwitcherCloseButtonClicked), this);
}




/*
 * EditAreaHolder class
 */

void EditAreaHolder::Init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/EditAreaHolder.ui");
    m_BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "EditAreaHolder"));
    m_Switcher = GTK_BOX(gtk_builder_get_object(builder, "Switcher"));
    m_Container = GTK_STACK(gtk_builder_get_object(builder, "Container"));
    gtk_widget_set_hexpand(GTK_WIDGET(m_BaseGrid), true);
}

void EditAreaHolder::Show(const shared_ptr<EditArea>& editarea){
    if(gtk_stack_get_child_by_name(m_Container, editarea->m_AbsoPath.c_str())==nullptr){
        // Edit area is not listed in this EditAreaHolder
        if(gtk_widget_get_parent(GTK_WIDGET(editarea->m_BaseGrid)) != nullptr){
            gtk_widget_unparent(GTK_WIDGET(editarea->m_BaseGrid));
        }
        gtk_stack_add_named(m_Container, GTK_WIDGET(editarea->m_BaseGrid),editarea->m_AbsoPath.c_str());
        m_TabButtons.emplace_back(make_shared<EditAreaHolderTabBut>());
        shared_ptr<EditAreaHolderTabBut> &b = m_TabButtons[m_TabButtons.size()-1];
        b->Init(editarea, *this);
        editarea->m_ParentSwitcher = b.get();
        gtk_box_append(m_Switcher, GTK_WIDGET(b->m_BaseBox));//Switcher
    }
    gtk_stack_set_visible_child_name(m_Container, editarea->m_AbsoPath.c_str());
    editarea->HighlightSyntax();
}

void EditAreaHolder::Remove(EditArea *editarea){
    /*
     * Remove tab switcher from holder
     */
    unsigned int offset = 0;
    for(std::shared_ptr<EditAreaHolderTabBut>& t: m_TabButtons){
        if(t.get()->m_CorrespondingEA.get() == editarea){
            gtk_box_remove(m_Switcher, GTK_WIDGET(t->m_BaseBox));
            m_TabButtons.erase(m_TabButtons.begin()+offset);
            break;
        }
        offset ++;
    }

    gtk_stack_remove(m_Container, GTK_WIDGET(editarea->m_BaseGrid));
}

std::shared_ptr<EditArea> EditAreaHolder::GetCurrentEditArea() {
    const char* visibleChildName = gtk_stack_get_visible_child_name(m_Container);
    if (visibleChildName) {
        for (const auto& tabButton : m_TabButtons) {
            if (tabButton->m_CorrespondingEA && tabButton->m_CorrespondingEA->m_AbsoPath == visibleChildName) {
                return tabButton->m_CorrespondingEA;
            }
        }
    }
    return nullptr;
}
