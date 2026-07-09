#include "Headerbar.h"

#include <gio/gmenu.h>
#include <gio/gmenumodel.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

#include "src/filemanagement/FileOperation.h"
#include "src/session/EditAreaData.h"
#include "Gui_if.h"
#include "AppUI.h"
#include "settingpanel/SettingPanel.h"



static void OnLoadFileClicked(GSimpleAction *action, GVariant *parameter, void* parent){
    filemanager::ChooseFile();
}

static void OnLoadFolderClicked(GSimpleAction *action, GVariant *parameter, void* parent){
    filemanager::ChooseFolder();
}

static void OnIdeButtonClicked(GtkButton *self, void* userdata){
    HeaderBar* headerbar = (HeaderBar*)userdata;
    headerbar->IdeButtonClicked();
}

static void OnNewFileClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    session::EditNewFile();
}

static void OnSearchClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    //if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
    //    gui::FocusedEAHolder->GetCurrentEditArea()->ShowSearchDialog();
    //}
}

static void OnReplaceClicked(GSimpleAction *action, GVariant *parameter, gpointer app) {
    //if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
    //    gui::FocusedEAHolder->GetCurrentEditArea()->ShowReplaceDialog();
    //}
}


static GActionEntry app_entries[] =
{
  { "file.open", OnLoadFileClicked, nullptr, nullptr, nullptr },
  { "folder.open", OnLoadFolderClicked, nullptr, nullptr, nullptr},
  { "file.new", OnNewFileClicked, nullptr, nullptr, nullptr },
  { "search.find", OnSearchClicked, nullptr, nullptr, nullptr },
  { "search.replace", OnReplaceClicked, nullptr, nullptr, nullptr }
};


HeaderBar::HeaderBar(AppUI& appui) :
    m_appUI(appui)
    {
    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/HeaderBar.ui");
    m_headerBarWidget = GTK_HEADER_BAR(gtk_builder_get_object(builder, "headerbar"));
    m_fileBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "file-btn"));
    m_ideBut = GTK_BUTTON(gtk_builder_get_object(builder, "app-btn"));
    g_signal_connect(m_ideBut, "clicked",G_CALLBACK(OnIdeButtonClicked), this);
    g_action_map_add_action_entries (G_ACTION_MAP (appui.gtkApp), app_entries, G_N_ELEMENTS (app_entries), appui.gtkApp);
}


void HeaderBar::IdeButtonClicked(){
    m_appUI.settingPanel->Show();
}

GtkWidget *HeaderBar::GetBaseWidget(){
    return GTK_WIDGET(m_headerBarWidget);
}
