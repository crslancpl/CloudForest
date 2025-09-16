#include "HeaderBar.h"
#include <gio/gmenu.h>
#include <gio/gmenumodel.h>

#include "../Core.h"
#include "../Global.h"

#include "EditArea.h"
#include "guiCore.h"
#include "MainWindow.h"
#include "SettingPanel.h"
#include "FilePanel.h"


static void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    /*
     * Tell file manager to open the file chooser
     */
    gui::OpenFileChooser(true);
}

static void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    /*
     * Tell file manager to open the file chooser and choose folder
     */
    gui::OpenFileChooser(false);
}

static void IdeButtonClicked(GtkButton *self, void* userdata){
    gui::AppSettingPanel.Show();
}

static void NewFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    auto newEditArea = gui::NewEditArea(nullptr);
    gui::FocusedEAHolder->Show(*newEditArea);
    gtk_widget_grab_focus(GTK_WIDGET((*newEditArea)->TextView));
}

static void SearchClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
        gui::FocusedEAHolder->GetCurrentEditArea()->ShowSearchDialog();
    }
}

static void ReplaceClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
        gui::FocusedEAHolder->GetCurrentEditArea()->ShowReplaceDialog();
    }
}


static GActionEntry app_entries[] =
{
  { "file.open", LoadFileClicked, nullptr, nullptr, nullptr },
  { "folder.open", LoadFolderClicked, nullptr, nullptr, nullptr },
  { "file.new", NewFileClicked, nullptr, nullptr, nullptr },
  { "search.find", SearchClicked, nullptr, nullptr, nullptr },
  { "search.replace", ReplaceClicked, nullptr, nullptr, nullptr }
};


void HeaderBar::Init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/HeaderBar.ui");

    HeaderBarWidget = GTK_HEADER_BAR(gtk_builder_get_object(builder, "HeaderBar"));
    FileBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "FileBut"));
    IdeBut = GTK_BUTTON(gtk_builder_get_object(builder, "AppButton"));

    g_signal_connect(IdeBut, "clicked",G_CALLBACK(IdeButtonClicked), nullptr);
    g_action_map_add_action_entries (G_ACTION_MAP (global::GtkApp), app_entries, G_N_ELEMENTS (app_entries), global::GtkApp);
}
