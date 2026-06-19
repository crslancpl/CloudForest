#include "Headerbar.h"

#include <gio/gmenu.h>
#include <gio/gmenumodel.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

#include "src/filemanagement/FileManagement_if.h"
#include "src/session/EditAreaData.h"
#include "Gui_if.h"
#include "settingpanel/SettingPanel.h"



static void LoadFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    filemanager::ChooseFile();
}

static void LoadFolderClicked(GSimpleAction *action, GVariant *parameter, gpointer app){
    filemanager::ChooseFolder();
}

static void IdeButtonClicked(GtkButton *self, void* userdata){
    gui::GetSettingPanel()->Show();
}

static void NewFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    session::EditNewFile();
}

static void SearchClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    //if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
    //    gui::FocusedEAHolder->GetCurrentEditArea()->ShowSearchDialog();
    //}
}

static void ReplaceClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    //if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
    //    gui::FocusedEAHolder->GetCurrentEditArea()->ShowReplaceDialog();
    //}
}


static GActionEntry app_entries[] =
{
  { "file.open", LoadFileClicked, nullptr, nullptr, nullptr },
  { "folder.open", LoadFolderClicked, nullptr, nullptr, nullptr},
  { "file.new", NewFileClicked, nullptr, nullptr, nullptr },
  { "search.find", SearchClicked, nullptr, nullptr, nullptr },
  { "search.replace", ReplaceClicked, nullptr, nullptr, nullptr }
};


HeaderBar::HeaderBar(GtkApplication* app){
    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/HeaderBar.ui");

    m_headerBarWidget = GTK_HEADER_BAR(gtk_builder_get_object(builder, "headerbar"));
    m_fileBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "file-btn"));
    m_ideBut = GTK_BUTTON(gtk_builder_get_object(builder, "app-btn"));

    g_signal_connect(m_ideBut, "clicked",G_CALLBACK(IdeButtonClicked), nullptr);
    g_action_map_add_action_entries (G_ACTION_MAP (app), app_entries, G_N_ELEMENTS (app_entries), app);
}

GtkWidget *HeaderBar::GetBaseWidget(){
    return GTK_WIDGET(m_headerBarWidget);
}
