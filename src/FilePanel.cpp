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

void FilePanel::NewFolder(GtkBox *Parent,GFile *File,GFile *ParentFolder){
    GtkBox *Box=GTK_BOX(gtk_box_new(GtkOrientation::GTK_ORIENTATION_VERTICAL, 5));
    gtk_box_append(Parent,GTK_WIDGET(Box) );
    char *p = g_file_get_relative_path(ParentFolder,File);
    g_print("%s\n",p);
    GtkButton *but= GTK_BUTTON(gtk_button_new());
    gtk_button_set_label(but, p);
    gtk_box_append(FileTree, GTK_WIDGET(but));
}


void FilePanel::init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
}
