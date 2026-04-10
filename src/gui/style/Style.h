#ifndef STYLE_H_
#define STYLE_H_

#include <gtk/gtk.h>
#include <string>

namespace style{
void LoadCssFile(const std::string &path);
void LoadCssFolder(const std::string &path);
void LoadTextTag(GtkTextBuffer *buffer);
}

#endif
