#include "FilePanel.h"

#include "FilePanelButtons.h"
#include "editarea/CompletionTool.h"
#include "src/AppContext.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>



WorkspaceBox::WorkspaceBox(Workspace& ws, AppContext& context) :
    m_ws(ws)
{
    m_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 1));
    gtk_widget_add_css_class(GTK_WIDGET(m_box), "ws-box");
    m_label = GTK_LABEL(gtk_label_new(ws.GetFileData()->fileName.c_str()));
    gtk_box_append(m_box, GTK_WIDGET(m_label));
    m_folderBtn = std::make_unique<FPFolderButton>(ws, 0, context);
    gtk_box_append(m_box, m_folderBtn->GetBaseWidget());
}

WorkspaceBox::~WorkspaceBox(){
    //
}

void WorkspaceBox::SetName(const char* name){
    m_ws.SetCustomName(name);
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
FilePanel::FilePanel(AppContext& context) :
    m_context(context)
    {
    GtkBuilder *builder =  gtk_builder_new_from_file("data/ui/FilePanel.ui");
    m_workspaceArea = GTK_BOX(gtk_builder_get_object(builder, "ws-area"));

    context.fileManager->EWorkspaceCreated.Connect(this, (void (EventReceiver::*)(Workspace*))&FilePanel::OnWorkspaceCreated);

    SetDefaultSize(270, 20);
    SetHorizontalExpand(false);
    SetVerticalExpand(true);
    g_object_ref(m_workspaceArea);
    gtk_widget_set_hexpand(GTK_WIDGET(m_workspaceArea), true);
    gtk_widget_set_vexpand(GTK_WIDGET(m_workspaceArea), true);
    gtk_box_set_spacing(m_workspaceArea, 5);
    gtk_widget_add_css_class(GTK_WIDGET(m_workspaceArea), "ws-area");
    this->SetContentWidget(GTK_WIDGET(m_workspaceArea));
    g_object_unref(builder);
}

FilePanel::~FilePanel(){
    for (std::unique_ptr<WorkspaceBox>& wsbox : m_workspaceList) {
        gtk_box_remove(m_workspaceArea, wsbox->GetBaseWidget());
    }

    m_workspaceList.clear();
}

void FilePanel::OnWorkspaceCreated(Workspace* ws){
    std::unique_ptr<WorkspaceBox> wsbox = std::make_unique<WorkspaceBox>(*ws, m_context);
    gtk_widget_add_css_class(wsbox->GetBaseWidget(), "workspace-box");
    gtk_box_append(m_workspaceArea, wsbox->GetBaseWidget());
    m_workspaceList.emplace_back(std::move(wsbox));
}
