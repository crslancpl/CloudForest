#include "DataTypes.h"
#include <cstddef>
#include <gio/gio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>


GFile *Parent;
void FilePanel::SetParent(GFile *File){
    Parent= File;
}
void FilePanel::NewFolder(GFile *File){
    char *p = g_file_get_relative_path(Parent,File);
    g_print("%s\n",p);
    GtkButton *but= GTK_BUTTON(gtk_button_new());
    gtk_button_set_label(but, p);
    //gtk_box_append(FileTree, GTK_WIDGET(but));

}


FilePanel::FilePanel(){
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "UI/FilePanel.ui",NULL);
    FileTree=GTK_BOX(gtk_builder_get_object(builder,"FileTree"));
    Grid=GTK_GRID(gtk_builder_get_object(builder, "FilePanel"));
}



FilePanel p;
 //gtk_builder_add_from_file(builder, "UI/FilePanel.ui", NULL);
 //p.FileTree=GTK_LIST_VIEW(gtk_builder_get_object(builder,"FileTree"));
 //p.Grid=GTK_GRID(gtk_builder_get_object(builder, "FilePanel"));
 //gtk_window_set_child(ThisWindow.Window, GTK_WIDGET(p.Grid));
