#include "HeaderBar.h"
#include <gio/gmenu.h>
#include <gio/gmenumodel.h>

#include "Core.h"
#include "FileManager.h"
#include "MainWindow.h"
#include "SettingPanel.h"

HeaderBar* LoadHeaderBar(GtkBuilder *builder){
    HeaderBar *NewHeaderBar = new HeaderBar();
    gtk_builder_add_from_file(builder, "UI/HeaderBar.ui", nullptr);

    NewHeaderBar->HeaderBarWidget = GTK_HEADER_BAR(gtk_builder_get_object(builder, "HeaderBar"));
    NewHeaderBar->FileBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "FileBut"));
    NewHeaderBar->IdeBut = GTK_BUTTON(gtk_builder_get_object(builder, "AppButton"));

    g_signal_connect(NewHeaderBar->IdeBut, "clicked",G_CALLBACK(IdeButtonClicked), nullptr);
    g_action_map_add_action_entries (G_ACTION_MAP (&GetApp()), app_entries, G_N_ELEMENTS (app_entries), &GetApp());

    return NewHeaderBar;
}

void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    OpenFileChooser(true);
}

void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    OpenFileChooser(false);
}

void IdeButtonClicked(GtkButton *self, void* userdata){
    ShowSettingPanel();
}
