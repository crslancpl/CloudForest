#include "FilePanelMenu.h"

#include "filepanel/FilePanel.h"
#include "filepanel/FilePanelButtons.h"
#include "AppUI.h"
#include "src/AppContext.h"

#include <gio/gio.h>
#include <gio/gmenu.h>
#include <gio/gmenumodel.h>
#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

/*
 * FilePanelMenu base class
 */

static void file_panel_menu_rename(GSimpleAction*, GVariant*, void* data){
    FilePanelButton* btn = (FilePanelButton*)data;
    btn->Rename();
}

static void file_panel_menu_delete(GSimpleAction*, GVariant*, void* data){
    FilePanelButton* btn = (FilePanelButton*)data;
    btn->Delete();
}

static GActionEntry file_panel_menu_entries[] = {
    {"fp.button.rename", file_panel_menu_rename, nullptr, nullptr, nullptr},
    {"fp.button.delete", file_panel_menu_delete, nullptr, nullptr, nullptr},
};

FilePanelMenu::FilePanelMenu(FilePanelButton& button, AppContext& context)
    : m_button(button){
    m_gMenu = g_menu_new();

    g_menu_append(m_gMenu, "Rename", "app.fp.button.rename");
    g_menu_append(m_gMenu, "Delete", "app.fp.button.delete");
    gtk_popover_menu_new_from_model(G_MENU_MODEL(m_gMenu));

    GtkApplication* app = context.appUI->GetGtkApp();
    g_action_map_add_action_entries(G_ACTION_MAP(app), file_panel_menu_entries, G_N_ELEMENTS(file_panel_menu_entries), &m_button);

    m_popover = GTK_POPOVER_MENU(gtk_popover_menu_new_from_model(G_MENU_MODEL(m_gMenu)));

    g_object_ref(m_popover);
}

FilePanelMenu::~FilePanelMenu(){
    gtk_widget_unparent(GTK_WIDGET(m_popover));
    g_object_unref(m_popover);
}

void FilePanelMenu::Show(){
    gtk_popover_popup(GTK_POPOVER(m_popover));
}

void FilePanelMenu::Hide(){
    gtk_popover_popdown(GTK_POPOVER(m_popover));
}



/*
 * FPFolderMenu class
 */

FPFolderMenu::FPFolderMenu(FPFolderButton& button, AppContext& context) : FilePanelMenu(button, context){
    gtk_widget_set_parent(GTK_WIDGET(m_popover), GTK_WIDGET(button.GetButton()));
    m_folderGMenu = g_menu_new();
    g_menu_append_section(m_gMenu, nullptr, G_MENU_MODEL(m_folderGMenu));
    g_menu_append(m_folderGMenu, "Reload", "fp.folder.reload");
    g_menu_append(m_folderGMenu, "New Folder", "fp.folder.newfolder");
    g_menu_append(m_folderGMenu, "New Folder", "fp.folder.newfile");
    g_object_unref(m_folderGMenu);
}

FPFolderMenu::~FPFolderMenu(){}


/*
 * FPFileMenu class
 */

FPFileMenu::FPFileMenu(FPFileButton& button, AppContext& context) : FilePanelMenu(button, context){
    gtk_widget_set_parent(GTK_WIDGET(m_popover), button.GetBaseWidget());
}

FPFileMenu::~FPFileMenu(){}
