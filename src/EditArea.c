#include "EditArea.h"

#include <glib/gprintf.h>
#include <glib.h>
#include <glibconfig.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ToolFunctions.h"

EditArea* edit_area_new(GtkBuilder *builder, GFile *File){
    gtk_builder_add_from_file(builder, "UI/EditArea.ui", NULL);

    // Create a place hold EditArea pointer;
    EditArea *NewEditArea = malloc(sizeof(EditArea));

    /* Binding */
    NewEditArea->BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));

    // FileInfo panel(top)
    NewEditArea->LocationBut = GTK_BUTTON(gtk_builder_get_object(builder,"but"));
    // Misc panel(bottom)
    NewEditArea->OutlineBut = GTK_BUTTON(gtk_builder_get_object(builder, "OutlineBut"));
    NewEditArea->ErrorBut = GTK_BUTTON(gtk_builder_get_object(builder, "ErrorBut"));
    NewEditArea->ErrorButLabel = GTK_LABEL(gtk_builder_get_object(builder, "ErrorButLabel"));
    NewEditArea->LangBut = GTK_BUTTON(gtk_builder_get_object(builder, "LangBut"));
    NewEditArea->CursorPosBut = GTK_BUTTON(gtk_builder_get_object(builder, "CursorPosBut"));
    // TextView
    NewEditArea->TextView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "TextArea"));
    NewEditArea->TextViewBuffer = gtk_text_view_get_buffer(NewEditArea->TextView);
    NewEditArea->LineNoArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "LineNum"));
    NewEditArea->LineNoAreaBuffer = gtk_text_view_get_buffer(NewEditArea->LineNoArea);

    // Initialize variables
    NewEditArea->cacheTotalLine = 0;
    NewEditArea->CursorPos = 0;

    NewEditArea->IsCurMovedByKey = false;
    NewEditArea->Cursoritr = malloc(sizeof(GtkTextIter));

    gtk_scrolled_window_set_vadjustment(
        GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "LineNoScroll")),
        gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(NewEditArea->TextView))
    );

    gtk_text_view_set_bottom_margin (NewEditArea->TextView, 80);
    gtk_text_view_set_bottom_margin (NewEditArea->LineNoArea, 80);

    gtk_text_view_set_pixels_below_lines(NewEditArea->TextView, 3);
    gtk_text_view_set_pixels_below_lines(NewEditArea->LineNoArea, 3);

    char *content;
    g_file_load_contents(File,NULL,&content, NULL, NULL,NULL);
    gtk_text_buffer_set_text(NewEditArea->TextViewBuffer, content, -1);
    g_free(content);

    CountLine(NewEditArea);
    CountError(NewEditArea);

    // Connect signals
    g_signal_connect(NewEditArea->TextView, "move-cursor", G_CALLBACK(CursorMovedByKey),NewEditArea);
    g_signal_connect(NewEditArea->TextViewBuffer, "notify::text",G_CALLBACK(TextChanged),NewEditArea);
    g_signal_connect_after(NewEditArea->TextViewBuffer, "notify::cursor-position",G_CALLBACK(CursorPosChanged),NewEditArea);

    return NewEditArea;
}

void DeleteEditAreaInstance(EditArea *instance){
    free(instance->Cursoritr);
    instance->Cursoritr = NULL;

    g_object_unref(instance->BaseGrid);//unref the top level container of EditArea

    free(instance);
    instance = NULL;

}

void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent){
    Parent->IsCurMovedByKey = true;
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

void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent){
    Parent->IsCurMovedByKey = true;
    CountLine(Parent);
}

void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent){
    g_object_get(buffer, "cursor-position",&Parent->CursorPos, NULL);
    gtk_text_buffer_get_iter_at_offset(buffer, Parent->Cursoritr, Parent->CursorPos);
    int LineNo = gtk_text_iter_get_line(Parent->Cursoritr) + 1;
    int LineOffset = gtk_text_iter_get_line_offset(Parent->Cursoritr) + 1;

    if(Parent->IsCurMovedByKey == true){
        gtk_text_view_scroll_to_iter(Parent->TextView, Parent->Cursoritr, 0.4 ,false, 0.4, 0.4);
        Parent->IsCurMovedByKey = false;
    }

    char Position[GetIntDigitCount(LineNo)+GetIntDigitCount(LineOffset)+GetIntDigitCount(Parent->cacheTotalLine)+16];
    sprintf(Position, "Line: %d/%d Offset: %d", LineNo, Parent->cacheTotalLine , LineOffset);
    gtk_button_set_label(Parent->CursorPosBut, Position);
}


void CountError(EditArea *Parent){
    int err = 0;
    int warn = 0;
    int info = 0;
    char s[75+GetIntDigitCount(err) + GetIntDigitCount(warn) + GetIntDigitCount(info)];
    sprintf(s, "<span color=\"red\">⚠%d</span> <span color=\"yellow\">⚠%d</span> <span color=\"greenyellow\">⚠%d</span>", err,warn,info);
    gtk_label_set_markup(Parent->ErrorButLabel, s);
}
