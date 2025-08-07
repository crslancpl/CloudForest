#ifndef EDITAREA_H_
#define EDITAREA_H_
#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>

#include "Classes.h"

using namespace std;

/* EdiaArea class is defined in DataTypes.h */

static void RemoveTagFromTable(GtkTextTag* tag, GtkTextTagTable* table);
static void AddTagToTable(GtkTextTag* tag,GtkTextTagTable* table);
static void ChooseLang(GtkButton *self, EditArea* Parent);
static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent);
static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent);
static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent);

static void SwitcherButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent);

#endif
