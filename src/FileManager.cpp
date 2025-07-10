#include "FileManager.h"

#include <gtk/gtk.h>

#include "EditArea.h"

GtkWindow *Parent;

void SetParentWindow(GtkWindow *parent){
    Parent = parent;
}

void OpenFileChooser(bool FileOrDir){
    // Pass True if select files. Pass False if select folder
    if(Parent == NULL){
        g_print("Use 'SetParentWindow()' to set the parent first");
        return;
    }
    GCancellable *Cancellable = g_cancellable_new();
    GtkFileDialog *FileDia = gtk_file_dialog_new();

    if(FileOrDir){
        gtk_file_dialog_set_title(FileDia, "Choose Files");
        gtk_file_dialog_open (FileDia, Parent, Cancellable, FileOpened, NULL);
    }else{
        gtk_file_dialog_set_title(FileDia, "Choose Folder");
        gtk_file_dialog_select_folder(FileDia, Parent, Cancellable, FileOpened, NULL);
    }
}

void FileOpened(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = NULL;
    File = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == NULL) return;
    EditArea *ea = edit_area_new(gtk_builder_new(), File);
    gtk_window_set_child(Parent, GTK_WIDGET(ea->BaseGrid));
}


void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = NULL;
    File = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == NULL) return;
    EditArea *ea = edit_area_new(gtk_builder_new(), File);
    gtk_window_set_child(Parent, GTK_WIDGET(ea->BaseGrid));
}
