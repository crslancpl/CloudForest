#include "DataTypes.h"
#include <gio/gio.h>
#include <glib.h>


GFile *Parent;
void FilePanel::SetParent(GFile *File){
    Parent= File;
}
void FilePanel::NewFolder(GFile *File){
    char *p = g_file_get_relative_path(Parent,File);
    g_print("%s\n",p);
}



FilePanel p;
 //gtk_builder_add_from_file(builder, "UI/FilePanel.ui", NULL);
 //p.FileTree=GTK_LIST_VIEW(gtk_builder_get_object(builder,"FileTree"));
 //p.Grid=GTK_GRID(gtk_builder_get_object(builder, "FilePanel"));
 //gtk_window_set_child(ThisWindow.Window, GTK_WIDGET(p.Grid));
