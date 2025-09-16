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

/*
 * EditArea class
 */

static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent){
    Parent->IsCurMovedByKey = true;
}

static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent){
    if(Parent->IsSaved == true){
        Parent->IsSaved = false;
        gtk_widget_remove_css_class(GTK_WIDGET(Parent->ParentSwitcher->Button), "SwitcherButtonSaved");
        gtk_widget_add_css_class(GTK_WIDGET(Parent->ParentSwitcher->Button), "SwitcherButtonUnsaved");
        gtk_button_set_label(Parent->SaveBut, "Save");
    }

    Parent->IsCurMovedByKey = true;
    Parent->CountLine();
    Parent->HighlightSyntax();

    //run the callbacks for python
    for (std::string &funcname : Parent->TextChangedPyCallback) {
        std::string code =
            funcname + "(\"" + Parent->AbsoPath + "\"," +
            std::to_string(Parent->CursorLine) + "," + std::to_string(Parent->CursorPos) +")";
        gui::PyRunCode(code);
    }
}

static bool KeyInput(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType state, EditArea* Parent){
    if(keyval == GDK_KEY_Tab){
        gtk_text_buffer_insert_at_cursor(Parent->TextViewBuffer, "    ", 4);
        return true;
    }

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
        }
    } else if (state & GDK_ALT_MASK) {
        if (keyval == GDK_KEY_s) {
            gui::AppSettingPanel.Show();
        }
    }

    return false;
}

static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent){
    Parent->LoadCursorPos();
    if(Parent->IsCurMovedByKey == true){
        gtk_text_view_scroll_to_iter(Parent->TextView, Parent->Cursoritr, 0.4 ,false, 0.4, 0.4);
        Parent->IsCurMovedByKey = false;
    }
}

static void SaveButtonClicked(GtkButton *self, EditArea* parent){
    parent->Save();
}



EditArea::EditArea(GFile *file){

    /* Loading EditArea from UI/EditArea.ui */
    builder = gtk_builder_new_from_file("UI/EditArea.ui");

    /* Binding */
    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    // FileInfo panel(top)
    LocationBut = GTK_BUTTON(gtk_builder_get_object(builder,"LocationBut"));
    SaveBut = GTK_BUTTON(gtk_builder_get_object(builder, "SaveBut"));
    // Misc panel(bottom)
    OutlineBut = GTK_BUTTON(gtk_builder_get_object(builder, "OutlineBut"));
    ErrorBut = GTK_BUTTON(gtk_builder_get_object(builder, "ErrorBut"));
    ErrorButLabel = GTK_LABEL(gtk_builder_get_object(builder, "ErrorButLabel"));
    LangBut = GTK_BUTTON(gtk_builder_get_object(builder, "LangBut"));
    CursorPosBut = GTK_BUTTON(gtk_builder_get_object(builder, "CursorPosBut"));
    // TextView
    TextView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "TextArea"));
    TextViewBuffer = gtk_text_view_get_buffer(TextView);
    LineNoArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "LineNum"));
    LineNoAreaBuffer = gtk_text_view_get_buffer(LineNoArea);
    KeyDownEventCtrl = gtk_event_controller_key_new();


    gtk_text_view_set_accepts_tab(TextView, false);

    /* Initialize variables */
    cacheTotalLine = 0;
    CursorPos = 0;
    GenerateId(RandomId);
    IsCurMovedByKey = false;
    Cursoritr = new GtkTextIter();
    StartItr = new GtkTextIter();
    EndItr = new GtkTextIter();

    /* Set properties */
    gtk_button_set_label(SaveBut, "Saved");
    gtk_scrolled_window_set_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "LineNoScroll")),
        gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(TextView))
    );
    gtk_text_view_set_bottom_margin (TextView, 80);
    gtk_text_view_set_bottom_margin (LineNoArea, 80);
    gtk_text_view_set_pixels_below_lines(TextView, 5);
    gtk_text_view_set_pixels_below_lines(LineNoArea, 5);

    /* Load file and other infos*/
    LoadFile(file);
    CountLine();
    CountError();
    LoadCursorPos();

    /* Connect signals */
    g_signal_connect(KeyDownEventCtrl, "key-pressed", G_CALLBACK(KeyInput), this);
    gtk_widget_add_controller(GTK_WIDGET(TextView), GTK_EVENT_CONTROLLER(KeyDownEventCtrl));
    g_signal_connect(TextView, "move-cursor", G_CALLBACK(CursorMovedByKey),this);
    g_signal_connect(TextViewBuffer, "notify::text",G_CALLBACK(TextChanged),this);
    g_signal_connect_after(TextViewBuffer, "notify::cursor-position",G_CALLBACK(CursorPosChanged),this);
    g_signal_connect(SaveBut, "clicked", G_CALLBACK(SaveButtonClicked), this);
    g_signal_connect(LangBut, "clicked", G_CALLBACK(style::OpenLangChooser), this);//Choose language is done by TextTag.cpp

    gtk_widget_set_has_tooltip(GTK_WIDGET(SaveBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(SaveBut), "Save");

    gtk_widget_set_has_tooltip(GTK_WIDGET(LocationBut), TRUE);
    gtk_widget_set_tooltip_text(GTK_WIDGET(LocationBut),
        file ? g_file_get_path(file) : "New file");


    style::LoadTextTag(TextViewBuffer);

    ChangeLanguage("cpp", false);
}

EditArea::~EditArea(){
    delete [] Cursoritr;
    delete [] StartItr;
    delete [] EndItr;
}

void EditArea::UnrefBuilder(){
    g_object_unref(builder);
}

void EditArea::CountLine(){
    int NewLineCount = gtk_text_buffer_get_line_count(TextViewBuffer);
    if(NewLineCount != cacheTotalLine){

        gtk_text_buffer_get_end_iter(LineNoAreaBuffer, StartItr);
        if(NewLineCount > cacheTotalLine){
            while(cacheTotalLine < NewLineCount){
                string Line = to_string(cacheTotalLine + 1) + '\n';
                gtk_text_buffer_insert(LineNoAreaBuffer, StartItr, Line.c_str(), -1);
                cacheTotalLine ++;
            }
        }else{
            gtk_text_buffer_get_iter_at_line(LineNoAreaBuffer, EndItr, NewLineCount);
            gtk_text_buffer_delete(LineNoAreaBuffer, EndItr, StartItr);
            cacheTotalLine = NewLineCount;
        }
    }
}

void EditArea::CountError(){
    int err = 0;
    int warn = 0;
    int info = 0;
    string s = "<span color=\"red\">⚠" + to_string(err) + "</span> <span color=\"yellow\">⚠" + to_string(warn)+ "</span> <span color=\"greenyellow\">⚠"+ to_string(info)+"</span>";
    gtk_label_set_markup(ErrorButLabel, s.c_str());
}

void EditArea::LoadCursorPos(){
    g_object_get(TextViewBuffer, "cursor-position", &CursorPos, nullptr);

    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, Cursoritr, CursorPos);
    CursorLine = gtk_text_iter_get_line(Cursoritr) + 1;
    CursorLinePos = gtk_text_iter_get_line_offset(Cursoritr) + 1;
    string Pos = "Line: " + to_string(CursorLine) + '/' + to_string(cacheTotalLine) + " Offset: " + to_string(CursorLinePos);
    gtk_button_set_label(CursorPosBut, Pos.c_str());
}

const std::string& EditArea::GetContent(){
    // To prevent the text to be discarded before the caller
    // get its content, we mark it static so it will not be discarded
    static std::string content;

    gtk_text_buffer_get_start_iter(TextViewBuffer, StartItr);
    gtk_text_buffer_get_end_iter(TextViewBuffer, EndItr);
    content = gtk_text_buffer_get_text(TextViewBuffer, StartItr, EndItr, true);
    return content;
}

void EditArea::AddTextChangedPyCalback(std::string funcname){
    TextChangedPyCallback.emplace_back(funcname);
}

void EditArea::ShowTip(char *Text){
    //
}

void EditArea::ShowSuggestion(const vector<shared_ptr<Suggestion>> &Suggestions){
    //
}


void EditArea::ChangeLanguage(const string &lang, bool highlight){
    gui::cfLoadLanguage(lang);
    Language = lang;

    emb_Send_Message_To_CF(MessageType::RELOAD, nullptr);
    if(highlight){
        HighlightSyntax();
    }
    gtk_button_set_label(LangBut, lang.c_str());

}

void EditArea::LoadFile(GFile* newfile){
    if (newfile == nullptr) {
        // Don't open file
        gtk_button_set_label(LocationBut, "New File");
        FileName = strdup("New File");
        AbsoPath = strdup("New File");
    }else{
        char *content;
        FileName = g_file_get_basename(newfile);
        AbsoPath = g_file_get_path(newfile);
        gtk_button_set_label(LocationBut, FileName.c_str());
        g_file_load_contents(newfile,nullptr,&content, nullptr, nullptr,nullptr);
        gtk_text_buffer_set_text(TextViewBuffer, content, -1);
        g_free(content);
        content = nullptr;
        EditingFile = newfile;
        if(ParentSwitcher != nullptr){
            gtk_button_set_label(ParentSwitcher->Button, FileName.c_str());
        }
    }
}

void EditArea::HighlightSyntax(){
    gtk_text_buffer_get_bounds(TextViewBuffer, StartItr, EndItr);
    gtk_text_buffer_remove_all_tags(TextViewBuffer, StartItr, EndItr);

    gui::cfProcessFile(AbsoPath, Language);
}

void EditArea::ApplyTagByLength(unsigned int TextStartPos, unsigned int TextLength, char *TagName){
    /*
     * Position 1 is the position of the first character in the buffer
     */
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, StartItr, TextStartPos);
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, StartItr, TextStartPos + TextLength);
    gtk_text_buffer_apply_tag_by_name(TextViewBuffer, TagName, StartItr, EndItr);
}

void EditArea::ApplyTagByPos(unsigned int TextStartPos, unsigned int TextEndPos, char *TagName){
    /*
     * Position 1 is the position of the first character in the buffer
     */
    TextStartPos --;
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, StartItr, TextStartPos);
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, EndItr, TextEndPos);
    gtk_text_buffer_apply_tag_by_name(TextViewBuffer, TagName, StartItr, EndItr);
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
    gtk_text_buffer_get_iter_at_line_offset(TextViewBuffer, StartItr, line, pos -1 );
    gtk_text_iter_assign(EndItr, StartItr);
    gtk_text_iter_forward_chars(EndItr, length);
    gtk_text_buffer_apply_tag_by_name(TextViewBuffer, TagName, StartItr, EndItr);
}

void EditArea::Destroy(){
    ParentHolder->Remove(this);
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
    EditAreacache->IsSaved = true;
    gtk_button_set_label(EditAreacache->SaveBut, "Saved");
    gtk_widget_remove_css_class(GTK_WIDGET(EditAreacache->ParentSwitcher->Button), "SwitcherButtonUnsaved");
    gtk_widget_add_css_class(GTK_WIDGET(EditAreacache->ParentSwitcher->Button), "SwitcherButtonSaved");
    EditAreacache = nullptr;
}

void EditArea::Save(){
    gtk_text_buffer_get_start_iter(TextViewBuffer, StartItr);
    gtk_text_buffer_get_end_iter(TextViewBuffer, EndItr);
    char* content = gtk_text_buffer_get_text(TextViewBuffer, StartItr, EndItr, true);

    EditAreacache = this;
    //async
    gui::SaveFile(EditingFile, content, FileSaved);
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
    if (Parent->EA != nullptr) {
        Parent->ParentHolder->Show(Parent->EA);
    }
}

static void SwitcherCloseButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent){
    gui::RemoveEditArea(Parent->EA.get());
}

void EditAreaHolderTabBut::Init(const shared_ptr<EditArea> &editarea, EditAreaHolder& parentholder){
    EA = editarea;
    ParentHolder = &parentholder;
    BaseBox = GTK_BOX(gtk_box_new(GtkOrientation::GTK_ORIENTATION_HORIZONTAL, 0));
    Button = GTK_BUTTON(gtk_button_new_with_label(editarea->FileName.c_str()));
    CloseButton = GTK_BUTTON(gtk_button_new_with_label("⛌"));
    gtk_widget_add_css_class(GTK_WIDGET(BaseBox), string("SwitcherBaseBox").c_str());
    gtk_widget_add_css_class(GTK_WIDGET(Button), string("SwitcherButtonSaved").c_str());
    gtk_widget_add_css_class(GTK_WIDGET(CloseButton), string("SwitcherCloseButton").c_str());
    gtk_box_append(BaseBox, GTK_WIDGET(Button));
    gtk_box_append(BaseBox, GTK_WIDGET(CloseButton));

    editarea->ParentSwitcher = this;
    editarea->ParentHolder = &parentholder;

    g_signal_connect(Button, "clicked", G_CALLBACK(SwitcherButtonClicked),this);
    g_signal_connect(CloseButton, "clicked", G_CALLBACK(SwitcherCloseButtonClicked), this);
}




/*
 * EditAreaHolder class
 */

void EditAreaHolder::Init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/EditAreaHolder.ui");
    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "EditAreaHolder"));
    Switcher = GTK_BOX(gtk_builder_get_object(builder, "Switcher"));
    Container = GTK_STACK(gtk_builder_get_object(builder, "Container"));
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), true);
}

void EditAreaHolder::Show(const shared_ptr<EditArea>& editarea){
    if(gtk_stack_get_child_by_name(Container, editarea->RandomId.c_str())==nullptr){
        // Edit area is not listed in this EditAreaHolder
        if(gtk_widget_get_parent(GTK_WIDGET(editarea->BaseGrid)) != nullptr){
            gtk_widget_unparent(GTK_WIDGET(editarea->BaseGrid));
        }
        gtk_stack_add_named(Container, GTK_WIDGET(editarea->BaseGrid),editarea->RandomId.c_str());
        TabButtons.emplace_back(make_shared<EditAreaHolderTabBut>());
        shared_ptr<EditAreaHolderTabBut> &b = TabButtons[TabButtons.size()-1];
        b->Init(editarea, *this);
        editarea->ParentSwitcher = b.get();
        gtk_box_append(Switcher, GTK_WIDGET(b->BaseBox));//Switcher
    }
    gtk_stack_set_visible_child_name(Container, editarea->RandomId.c_str());
    editarea->HighlightSyntax();
}

void EditAreaHolder::Remove(EditArea *editarea){
    /*
     * Remove tab switcher from holder
     */
    unsigned int offset = 0;
    for(std::shared_ptr<EditAreaHolderTabBut>& t: TabButtons){
        if(t.get()->EA.get() == editarea){
            gtk_box_remove(Switcher, GTK_WIDGET(t->BaseBox));
            TabButtons.erase(TabButtons.begin()+offset);
            break;
        }
        offset ++;
    }

    gtk_stack_remove(Container, GTK_WIDGET(editarea->BaseGrid));
}

std::shared_ptr<EditArea> EditAreaHolder::GetCurrentEditArea() {
    const char* visibleChildName = gtk_stack_get_visible_child_name(Container);
    if (visibleChildName) {
        for (const auto& tabButton : TabButtons) {
            if (tabButton->EA && tabButton->EA->RandomId == visibleChildName) {
                return tabButton->EA;
            }
        }
    }
    return nullptr;
}
