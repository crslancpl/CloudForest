#ifndef STYLE_H_
#define STYLE_H_

#include <gtk/gtk.h>
#include <string>
#include <list>

namespace style{
void LoadCssFile(const std::string &path);
void LoadCssFolder(const std::string &path);
void LoadTextTag(GtkTextBuffer *buffer);//two tag name lists can be null
const std::list<const char*>& GetTextTagNamesForHighlight();
const std::list<const char*>& GetTextTagNamesForDiagnostic();
}

#endif
