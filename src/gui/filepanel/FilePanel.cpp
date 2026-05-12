#include "FilePanel.h"

#include "FilePanelButtons.h"
#include "datatypes/file.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>



WorkspaceBox::WorkspaceBox(Workspace* ws): m_ws(ws){
    m_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 1));
    gtk_widget_add_css_class(GTK_WIDGET(m_box), "ws-box");
    m_label = GTK_LABEL(gtk_label_new(ws->name));
    gtk_box_append(m_box, GTK_WIDGET(m_label));
    FPFolderButton* folderbtn = new FPFolderButton(ws->wsBranch, 0);
    gtk_box_append(m_box, folderbtn->GetBaseWidget());
}

void WorkspaceBox::SetName(const char* name){
    m_ws->name = strdup(name);
    gtk_label_set_text(m_label, name);
}

GtkWidget* WorkspaceBox::GetBaseWidget(){
    return GTK_WIDGET(m_box);
}

/*
 * FilePanel class
 */
unsigned short FilePanel::Offset = 20;


// public
FilePanel::FilePanel(){
    GtkBuilder *builder =  gtk_builder_new_from_file("data/ui/FilePanel.ui");
    m_workspaceArea = GTK_BOX(gtk_builder_get_object(builder, "ws-area"));

    SetDefaultSize(270, 20);
    SetHorizontalExpand(false);
    SetVerticalExpand(true);
    gtk_widget_set_hexpand(GTK_WIDGET(m_workspaceArea), true);
    gtk_widget_set_vexpand(GTK_WIDGET(m_workspaceArea), true);
    gtk_box_set_spacing(m_workspaceArea, 5);
    gtk_widget_add_css_class(GTK_WIDGET(m_workspaceArea), "ws-area");
    SetContentWidget(GTK_WIDGET(m_workspaceArea));
    g_object_unref(builder);
}

void FilePanel::NewWorkspace(Workspace* ws){
    WorkspaceBox* wsbox = new WorkspaceBox(ws);
    gtk_widget_add_css_class(wsbox->GetBaseWidget(), "workspace-box");
    m_workspaceList.emplace(wsbox);
    gtk_box_append(m_workspaceArea, wsbox->GetBaseWidget());
}
