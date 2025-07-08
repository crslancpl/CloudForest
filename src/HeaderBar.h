#ifndef HEADERBAR_H_
#define HEADERBAR_H_
#pragma once

#include <gtk/gtk.h>

typedef struct HeaderBar{
    GtkApplication *App;
    GtkHeaderBar *HeaderBar;
    GtkMenuButton *FileBut;
}HeaderBar;

HeaderBar* LoadHeaderBar(GtkBuilder *builder, GtkApplication *app);


void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app);

static GActionEntry app_entries[] =
{
  { "file.open", LoadFileClicked, NULL, NULL, NULL },
  { "folder.open", LoadFileClicked, NULL, NULL, NULL }
};

#endif
