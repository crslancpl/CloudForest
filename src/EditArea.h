#ifndef EDITAREA_H_
#define EDITAREA_H_
#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>

#include "DataTypes.h"

using namespace std;

static void ChooseLang(GtkButton *self, EditArea* Parent);
static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent);
static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent);
static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent);

#endif
