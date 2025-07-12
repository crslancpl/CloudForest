#include "FileManager.h"

#include <cstddef>
#include <gtk/gtk.h>
#include <memory>

#include "EditArea.h"
#include "DataTypes.h"

GtkWindow *Parent;
GtkFileDialog *FileDia;
GCancellable *Cancellable;

using namespace std;

void InitFileManager(GtkWindow *parent){
    g_print("init File manager\n");
    Parent = parent;
    Cancellable = g_cancellable_new();
    FileDia = gtk_file_dialog_new();
}

void OpenFileChooser(bool FileOrDir){
    // Pass True if select files. Pass False if select folder
    if(Parent == NULL){
        g_print("Use 'SetParentWindow()' to set the parent first");
        return;
    }

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
    if(File == NULL) {
        // cancelled
        g_print("Cancelled\n");
        free(File);
        return;
    }


    shared_ptr<EditArea> ea = make_shared<EditArea>(File);
    SectionData::AddEditArea(ea);
    gtk_window_set_child(Parent, GTK_WIDGET(ea->BaseGrid));

    ea->UnrefBuilder();
}


void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    //GFile *File;
    //GError *err = NULL;
    //File = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    //if(File == NULL) return;
    //EditArea *ea = edit_area_new(gtk_builder_new(), File);
    //gtk_window_set_child(Parent, GTK_WIDGET(ea->BaseGrid));
}
