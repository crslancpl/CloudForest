#ifndef HEADERBAR_H_
#define HEADERBAR_H_
#pragma once

#include <gtk/gtk.h>

#include "Classes.h"

class HeaderBar{
public:
    GtkHeaderBar *HeaderBarWidget;
    GtkMenuButton *FileBut;
    // Other buttons is not funtioning yet
    GtkButton *CompileBut;
    GtkButton *IdeBut;
    GtkSearchEntry *SearchBar;
};

HeaderBar* LoadHeaderBar(GtkBuilder *builder);//share builder with MainWindow

void IdeButtonClicked(GtkButton *self, void* userdata);
void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app);
void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app);
void NewFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app); // To create a new file

static GActionEntry app_entries[] =
{
  { "file.open", LoadFileClicked, nullptr, nullptr, nullptr },
  { "folder.open", LoadFolderClicked, nullptr, nullptr, nullptr },
  { "file.new", NewFileClicked, nullptr, nullptr, nullptr }
};

#endif
