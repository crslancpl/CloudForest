#ifndef EDITAREA_H_
#define EDITAREA_H_
#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

class EditArea{
public:
    char *FileName;
    int cacheTotalLine;
    int CursorPos;
    GtkTextIter *Cursoritr;
    bool IsCurMovedByKey;

    GtkGrid *BaseGrid;

    GtkButton *LocationBut;
    GtkButton *ErrorBut;
    GtkLabel *ErrorButLabel;
    GtkButton *OutlineBut;
    GtkButton *LangBut;
    GtkButton *CursorPosBut;

    GtkTextView *TextView;
    GtkTextBuffer *TextViewBuffer;
    GtkTextView *LineNoArea;
    GtkTextBuffer *LineNoAreaBuffer;
};


EditArea* edit_area_new(GtkBuilder *builder, GFile *File);
void DeleteEditAreaInstance(EditArea *instance);

static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent);
static void CountLine(EditArea *Parent);
static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent);
static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent);
static void CountError(EditArea *Parent);

#endif
