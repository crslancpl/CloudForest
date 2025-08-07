#include "EditArea.h"

// <gtk/gtk.h> is included in EditArea.h
#include <cstddef>
#include <gio/gmenu.h>
#include <glib/gprintf.h>
#include <glib.h>
#include <glibconfig.h>

#include <gtk/gtk.h>
#include <memory>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>

#include "ToolFunctions.h"
#include "Classes.h"

EditArea::EditArea(GFile *File){

    builder = gtk_builder_new_from_file("UI/EditArea.ui");

    /* Binding */
    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));

    // FileInfo panel(top)
    LocationBut = GTK_BUTTON(gtk_builder_get_object(builder,"LocationBut"));
    // Misc panel(bottom)
    OutlineBut = GTK_BUTTON(gtk_builder_get_object(builder, "OutlineBut"));
    ErrorBut = GTK_BUTTON(gtk_builder_get_object(builder, "ErrorBut"));
    ErrorButLabel = GTK_LABEL(gtk_builder_get_object(builder, "ErrorButLabel"));
    LangBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "LangBut"));
    LangMenu = G_MENU(gtk_builder_get_object(builder, "LangMenu"));
    CursorPosBut = GTK_BUTTON(gtk_builder_get_object(builder, "CursorPosBut"));
    // TextView
    TextView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "TextArea"));
    TextViewBuffer = gtk_text_view_get_buffer(TextView);
    LineNoArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "LineNum"));
    LineNoAreaBuffer = gtk_text_view_get_buffer(LineNoArea);

    // Dialog
    win = GTK_WINDOW(gtk_builder_get_object(builder, "dialog1"));

    // Initialize variables
    cacheTotalLine = 0;
    CursorPos = 0;

    IsCurMovedByKey = false;

    Cursoritr = new GtkTextIter();
    StartItr = new GtkTextIter();
    EndItr = new GtkTextIter();

    gtk_scrolled_window_set_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "LineNoScroll")),
        gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(TextView))
    );

    gtk_text_view_set_bottom_margin (TextView, 80);
    gtk_text_view_set_bottom_margin (LineNoArea, 80);

    gtk_text_view_set_pixels_below_lines(TextView, 5);
    gtk_text_view_set_pixels_below_lines(LineNoArea, 5);


    if (File == NULL) {
        // Don't open file
        g_print("NULL gfile\n");
        gtk_button_set_label(LocationBut, "New File");
        FileName = "New File";
        AbsoPath = "New File";
    }else{
        char *content;
        FileName = g_file_get_basename(File);
        AbsoPath = g_file_get_path(File);
        //g_print("%s", FileName);
        gtk_button_set_label(LocationBut, FileName);
        g_file_load_contents(File,NULL,&content, NULL, NULL,NULL);
        gtk_text_buffer_set_text(TextViewBuffer, content, -1);
        g_free(content);
        content = NULL;
    }



    CountLine();
    CountError();
    LoadCursorPos();

    // Connect signals
    g_signal_connect(TextView, "move-cursor", G_CALLBACK(CursorMovedByKey),this);
    g_signal_connect(TextViewBuffer, "notify::text",G_CALLBACK(TextChanged),this);
    g_signal_connect_after(TextViewBuffer, "notify::cursor-position",G_CALLBACK(CursorPosChanged),this);
    //g_signal_connect(LangBut, "clicked", G_CALLBACK(ChooseLang), this);

    Lang = "cf";

    //ChangeLanguage();
}

EditArea::~EditArea(){
    free(Cursoritr);
    Cursoritr = NULL;
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
    g_object_get(TextViewBuffer, "cursor-position", &CursorPos, NULL);

    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, Cursoritr, CursorPos);
    int LineNo = gtk_text_iter_get_line(Cursoritr) + 1;
    int LineOffset = gtk_text_iter_get_line_offset(Cursoritr) + 1;
    string Pos = "Line: " + to_string(LineNo) + '/' + to_string(cacheTotalLine) + " Offset: " + to_string(LineOffset);
    gtk_button_set_label(CursorPosBut, Pos.c_str());
}

void EditArea::ShowTip(char *Text){
    //
}

void EditArea::ShowSuggestion(const vector<shared_ptr<Suggestion>> &Suggestions){
    //
}

void EditArea::ChangeLanguage(){
    //
    gtk_text_buffer_get_start_iter(TextViewBuffer, StartItr);
    gtk_text_buffer_get_end_iter(TextViewBuffer, EndItr);
    gtk_text_buffer_remove_all_tags(TextViewBuffer, StartItr, EndItr);
    GtkTextTagTable *t = gtk_text_buffer_get_tag_table(TextViewBuffer);
    gtk_text_tag_table_foreach(t, GtkTextTagTableForeach(RemoveTagFromTable),t);
    for(TagStyle style:TagTables::GetLangTagTable(Lang)->LangStyles){
        g_print("%s\n", style.TagName.c_str());
        GtkTextTag *tag = gtk_text_tag_new(style.TagName.c_str());
        for ( pair<string, string> s: style.Styles) {
            GValue *v = new GValue;
            *v = G_VALUE_INIT;
            g_value_init(v, G_TYPE_STRING);
            g_value_set_string(v, s.second.c_str());
            g_object_set_property(G_OBJECT(tag), s.first.c_str(), v);
        }
        gtk_text_tag_table_add(t, tag);
    }
    g_print("Tags count %d", gtk_text_tag_table_get_size(t));
}

void EditArea::ApplyTagByLength(int TextStartPos, int TextLength, char *TagName){
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, StartItr, TextStartPos);
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, StartItr, TextStartPos + TextLength);
    gtk_text_buffer_apply_tag_by_name(TextViewBuffer, TagName, StartItr, EndItr);
}

void EditArea::ApplyTagByPos(int TextStartPos, int TextEndPos, char *TagName){
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, StartItr, TextStartPos);
    gtk_text_buffer_get_iter_at_offset(TextViewBuffer, EndItr, TextEndPos);
    gtk_text_buffer_apply_tag_by_name(TextViewBuffer, TagName, StartItr, EndItr);
}

void RemoveTagFromTable(GtkTextTag* tag,GtkTextTagTable* table){
    g_print("removing tag\n");
    gtk_text_tag_table_remove(table, tag);
}

void ChooseLang(GtkButton *self, EditArea* Parent){
    gtk_widget_set_visible(GTK_WIDGET(Parent->win), true);
}

void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent){
    Parent->IsCurMovedByKey = true;
}

void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent){
    Parent->IsCurMovedByKey = true;
    Parent->CountLine();
}

void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent){
    Parent->LoadCursorPos();
    if(Parent->IsCurMovedByKey == true){
        gtk_text_view_scroll_to_iter(Parent->TextView, Parent->Cursoritr, 0.4 ,false, 0.4, 0.4);
        Parent->IsCurMovedByKey = false;
    }
}





void EditAreaHolderTabBut::Init(const shared_ptr<EditArea> &editarea, EditAreaHolder& parentholder){
    EA = editarea;
    ParentHolder = &parentholder;
    Button = GTK_BUTTON(gtk_button_new_with_label(editarea->FileName));
}

void SwitcherButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent){
    Parent->ParentHolder->Show(Parent->EA);
}


void EditAreaHolder::Init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/EditArea.ui");
    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "EditAreaHolder"));
    Switcher = GTK_BOX(gtk_builder_get_object(builder, "Switcher"));
    Container = GTK_STACK(gtk_builder_get_object(builder, "Container"));
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), true);
}

void EditAreaHolder::Show(const shared_ptr<EditArea>& editarea){

    if(gtk_stack_get_child_by_name(Container, editarea->AbsoPath)==NULL){
        // Edit area is not listed in this EditAreaHolder

        if(editarea->BaseGrid == NULL)abort();
        if(gtk_widget_get_parent(GTK_WIDGET(editarea->BaseGrid)) != NULL){
            gtk_widget_unparent(GTK_WIDGET(editarea->BaseGrid));
        }

        gtk_stack_add_named(Container, GTK_WIDGET(editarea->BaseGrid),editarea->AbsoPath);
        TabButtons.emplace_back(make_shared<EditAreaHolderTabBut>());
        shared_ptr<EditAreaHolderTabBut> &b = TabButtons[TabButtons.size()-1];
        b->Init(editarea, *this);
        gtk_box_append(Switcher, GTK_WIDGET(b->Button));//Switcher
        EditAreaHolderTabBut *edhtb = b.get();
        g_signal_connect(b->Button, "clicked", G_CALLBACK(SwitcherButtonClicked), edhtb);
    }

    gtk_stack_set_visible_child_name(Container, editarea->AbsoPath);

}
