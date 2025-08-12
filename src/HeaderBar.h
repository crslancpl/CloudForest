#ifndef HEADERBAR_H_
#define HEADERBAR_H_
#pragma once

#include <gtk/gtk.h>

#include "Classes.h"

class HeaderBar{
public:
    GtkApplication *App;
    GtkHeaderBar *HeaderBar;
    GtkMenuButton *FileBut;
};

HeaderBar* LoadHeaderBar(GtkBuilder *builder, GtkApplication *app);


void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app);
void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app);

static GActionEntry app_entries[] =
{
  { "file.open", LoadFileClicked, nullptr, nullptr, nullptr },
  { "folder.open", LoadFolderClicked, nullptr, nullptr, nullptr }
};

#endif
