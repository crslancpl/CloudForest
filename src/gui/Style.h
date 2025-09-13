#ifndef STYLE_H_
#define STYLE_H_

#include <gtk/gtk.h>
#include <string>

#include "EditArea.h"

namespace style{
void LoadCssFromPath(const std::string &path);
void LoadTextTag(GtkTextBuffer *buffer);
void InitLangChooser();
void OpenLangChooser(GtkButton *self, EditArea *parent);
}

#endif
