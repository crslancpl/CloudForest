#include "DataTypes.h"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>


GFile *Parent;

void FilePanel::SetParent(GFile *File){
    Parent = File;
}

void FilePanel::NewFolder(GFile *File){
    char *p = g_file_get_relative_path(Parent,File);
    g_print("%s\n",p);
    GtkButton *but= GTK_BUTTON(gtk_button_new());
    gtk_button_set_label(but, p);
    gtk_box_append(FileTree, GTK_WIDGET(but));
}

FilePanel::FilePanel(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/FilePanel.ui");

    BaseGrid = GTK_GRID(gtk_builder_get_object(builder, "BaseGrid"));
    FileTree = GTK_BOX(gtk_builder_get_object(builder, "FileTree"));
}
