#ifndef EDITAREA_H_
#define EDITAREA_H_
#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

typedef struct EditArea{
    int cacheTotalLine;
    GtkGrid *BaseGrid;

    GtkButton *LocationBut;
    GtkButton *But;
    GtkButton *ErrorBut;
    GtkButton *OutlineBut;
    GtkButton *LangBut;
    GtkButton *CursorPosBut;

    GtkTextView *TextView;
    GtkTextBuffer *TextViewBuffer;
    GtkTextView *LineNoArea;
    GtkTextBuffer *LineNoAreaBuffer;
}EditArea;

void DeleteEditAreaInstance(EditArea *instance);

EditArea* edit_area_new();

#endif
