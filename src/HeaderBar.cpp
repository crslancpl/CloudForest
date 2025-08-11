#include "HeaderBar.h"
#include <gio/gmenu.h>
#include <gio/gmenumodel.h>

#include "FileManager.h"

HeaderBar* LoadHeaderBar(GtkBuilder *builder, GtkApplication *app){
    HeaderBar *NewHeaderBar = new HeaderBar();
    gtk_builder_add_from_file(builder, "UI/HeaderBar.ui", NULL);

    NewHeaderBar->App = app;
    NewHeaderBar->HeaderBar = GTK_HEADER_BAR(gtk_builder_get_object(builder, "HeaderBar"));
    NewHeaderBar->FileBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "FileBut"));

    g_action_map_add_action_entries (G_ACTION_MAP (NewHeaderBar->App), app_entries, G_N_ELEMENTS (app_entries), NewHeaderBar->App);

    return NewHeaderBar;
}

void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    OpenFileChooser(true);
}

void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    OpenFileChooser(false);
}
