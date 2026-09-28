#include "Headerbar.h"

#include <gio/gio.h>
#include <gio/gmenu.h>
#include <gio/gmenumodel.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

#include "src/AppContext.h"
#include "src/filemanagement/FileManager.h"
#include "src/session/EditAreaData.h"
#include "AppUI.h"
#include "settingpanel/SettingPanel.h"



static void on_open_file_clicked(GSimpleAction *action, GVariant *parameter, void* parent){
    ((HeaderBar*)parent)->OnLoadFileClicked();
}

static void on_open_folder_clicked(GSimpleAction *action, GVariant *parameter, void* parent){
    ((HeaderBar*)parent)->OnLoadFolderClicked();
}

static void on_ide_button_clicked(GtkButton *self, void* parent){
    ((HeaderBar*)parent)->OnIdeButtonClicked();
}

static void on_new_file_clicked(GSimpleAction *action, GVariant *parameter, void* parent) {
    session::EditNewFile();
}

static void on_search_clicked(GSimpleAction *action, GVariant *parameter, void* parent) {
    //if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
    //    gui::FocusedEAHolder->GetCurrentEditArea()->ShowSearchDialog();
    //}
}

static void on_replace_clicked(GSimpleAction *action, GVariant *parameter, void* parent) {
    //if (gui::FocusedEAHolder && gui::FocusedEAHolder->GetCurrentEditArea()) {
    //    gui::FocusedEAHolder->GetCurrentEditArea()->ShowReplaceDialog();
    //}
}




HeaderBar::HeaderBar(AppContext& context) :
    m_context(context)
    {
    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/HeaderBar.ui");
    m_headerBarWidget = GTK_HEADER_BAR(gtk_builder_get_object(builder, "headerbar"));
    m_fileBut = GTK_MENU_BUTTON(gtk_builder_get_object(builder, "file-btn"));
    m_ideBut = GTK_BUTTON(gtk_builder_get_object(builder, "app-btn"));
    g_signal_connect(m_ideBut, "clicked",G_CALLBACK(on_ide_button_clicked), this);

    GActionEntry app_entries[] =
    {
      { "file.open", on_open_file_clicked, nullptr, nullptr, nullptr},
      { "folder.open", on_open_folder_clicked, nullptr, nullptr, nullptr},
      { "file.new", on_new_file_clicked, nullptr, nullptr, nullptr },
      { "search.find", on_search_clicked, nullptr, nullptr, nullptr },
      { "search.replace", on_replace_clicked, nullptr, nullptr, nullptr }
    };

    g_action_map_add_action_entries (
        G_ACTION_MAP (m_context.appUI->GetGtkApp()),
        app_entries, G_N_ELEMENTS (app_entries),
        this
    );
}

void HeaderBar::OnLoadFileClicked(){
    m_context.fileManager->ChooseFile();
}

void HeaderBar::OnLoadFolderClicked(){
    m_context.fileManager->ChooseFolder();
}

void HeaderBar::OnIdeButtonClicked(){
    m_context.appUI->GetSettingPanel()->Show();
}

GtkWidget *HeaderBar::GetBaseWidget(){
    return GTK_WIDGET(m_headerBarWidget);
}
