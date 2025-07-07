#include "EditArea.h"

#include <glib/gprintf.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ToolFunctions.h"

void DeleteEditAreaObject(){
    //free();
}

void DeleteEditAreaInstance(EditArea *instance){
    free(instance);
}

void L(GtkTextView *T, GtkMovementStep step, gint count, gboolean e_s, gpointer UserData){
    g_print("L\n");
}

void G(GtkButton *B){
    g_print("g\n");
}

void C(GtkTextBuffer *T, gpointer user_data){
    g_print("c\n");
}

void B (GtkTextBuffer *T,  gpointer user_data){
    g_print("b\n");
}

void CountLine(EditArea *Parent){
    int NewLineCount = gtk_text_buffer_get_line_count(Parent->TextViewBuffer);
    if(NewLineCount != Parent->cacheTotalLine){
        GtkTextIter *Firstitr = malloc(sizeof(GtkTextIter));
        gtk_text_buffer_get_end_iter(Parent->LineNoAreaBuffer, Firstitr);
        if(NewLineCount > Parent->cacheTotalLine){
            while(Parent->cacheTotalLine < NewLineCount){
                char LineName[GetIntDigitCount(Parent->cacheTotalLine)];
                sprintf(LineName, "%d\n", Parent->cacheTotalLine+1);
                gtk_text_buffer_insert(Parent->LineNoAreaBuffer, Firstitr, LineName, -1);
                Parent->cacheTotalLine ++;
            }
        }else{
            GtkTextIter *NewEnditr = malloc(sizeof(GtkTextIter));
            gtk_text_buffer_get_iter_at_line(Parent->LineNoAreaBuffer, NewEnditr, NewLineCount);
            gtk_text_buffer_delete(Parent->LineNoAreaBuffer, NewEnditr, Firstitr);
            free(NewEnditr);
            NewEnditr= NULL;
            Parent->cacheTotalLine = NewLineCount;
        }
        free(Firstitr);
        Firstitr=NULL;
    }
}

void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent)
{
    CountLine(Parent);
}

void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent)
{
    int CursorPos;
    GtkTextIter *itr = malloc(sizeof(GtkTextIter));
    g_object_get(buffer, "cursor-position",&CursorPos, NULL);
    gtk_text_buffer_get_iter_at_offset(buffer, itr, CursorPos);
    int LineNo = gtk_text_iter_get_line(itr) + 1;
    int LineOffset = gtk_text_iter_get_line_offset(itr) + 1;
    gtk_text_view_scroll_to_iter(Parent->TextView, itr, 0.1 ,false, 0.2, 0.3);
    free(itr);
    itr = NULL;
    char Position[GetIntDigitCount(LineNo)+GetIntDigitCount(LineOffset)+GetIntDigitCount(Parent->cacheTotalLine)+16];
    sprintf(Position, "Line: %d/%d Offset: %d", LineNo, Parent->cacheTotalLine , LineOffset);
    gtk_button_set_label(Parent->CursorPosBut, Position);
}

EditArea* edit_area_new(){
    g_print("New edit area");
    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "UI/EditArea.ui", NULL);
    EditArea *NewEditArea = malloc(sizeof(EditArea));

    NewEditArea->cacheTotalLine = 0;

    NewEditArea->BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));

    NewEditArea->LocationBut = GTK_BUTTON(gtk_builder_get_object(builder,"but"));

    NewEditArea->LangBut = GTK_BUTTON(gtk_builder_get_object(builder, "LangBut"));
    NewEditArea->CursorPosBut = GTK_BUTTON(gtk_builder_get_object(builder, "CursorPosBut"));

    NewEditArea->TextView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "TextArea"));
    NewEditArea->TextViewBuffer = gtk_text_view_get_buffer(NewEditArea->TextView);
    NewEditArea->LineNoArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "LineNum"));
    NewEditArea->LineNoAreaBuffer = gtk_text_view_get_buffer(NewEditArea->LineNoArea);

    CountLine(NewEditArea);


    gtk_scrolled_window_set_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "LineNoScroll")),
        gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(NewEditArea->TextView))
    );


    gtk_text_view_set_bottom_margin (NewEditArea->TextView, 80);
    gtk_text_view_set_bottom_margin (NewEditArea->LineNoArea, 80);

    gtk_text_view_set_pixels_below_lines(NewEditArea->TextView, 3);
    gtk_text_view_set_pixels_below_lines(NewEditArea->LineNoArea, 3);


    g_signal_connect(NewEditArea->LocationBut, "clicked", G_CALLBACK(G), NULL);
    g_signal_connect_after(NewEditArea->TextView, "notify::move-cusor",G_CALLBACK(L),NULL);
    g_signal_connect(NewEditArea->TextViewBuffer, "notify::text",G_CALLBACK(TextChanged),NewEditArea);
    g_signal_connect(NewEditArea->TextViewBuffer, "notify::cursor-position",G_CALLBACK(CursorPosChanged),NewEditArea);
    g_signal_connect(NewEditArea->TextViewBuffer, "notify::begin-user-action",G_CALLBACK(B),NULL);

    return NewEditArea;
}
