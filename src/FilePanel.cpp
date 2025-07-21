#include "DataTypes.h"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>


GFile *RootFolder;

void FilePanel::SetParent(GFile *File){
    RootFolder = File;
}

void FilePanel::NewFolder(Folder *Parent,GFile *File,GFile *ParentFolder){
    Folder *Child=(Folder *)malloc(sizeof(Folder));
    Child->init(File, ParentFolder);
    Parent->AddChildFolder(Child);
}


void FilePanel::init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
    gtk_widget_set_size_request(GTK_WIDGET(BaseGrid), 200, 20);
    gtk_widget_set_hexpand(GTK_WIDGET(BaseGrid), false);
}
void Folder::init(GFile *Folder,GFile *Parent){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "FolderBaseGrid"));
    FolderToggleBut = GTK_BUTTON(gtk_builder_get_object(builder, "FolderToggleBut"));
    Content = GTK_BOX(gtk_builder_get_object(builder, "Content"));
    FolderName = g_file_get_relative_path(Parent,Folder);
    gtk_button_set_label(FolderToggleBut, FolderName);


}
void Folder::AddChildFolder(Folder *Child){
    gtk_box_append(Content, GTK_WIDGET(Child->BaseGrid));
}
