#include "FileManager.h"

#include <cstddef>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>

#include "Classes.h"
#include "EditArea.h"
#include "FilePanel.h"
#include "SectionData.h"

MainWindow *ParentWindow;
GtkFileDialog *FileDia;

using namespace std;

void InitFileManager(MainWindow *parent){
    g_print("init File manager\n");
    ParentWindow = parent;
    FileDia = gtk_file_dialog_new();
}

void OpenFileChooser(bool FileOrDir){
    // Pass True if select files. Pass False if select folder
    if(ParentWindow->Window == NULL){
        g_print("Use 'SetParentWindow()' to set the parent first");
        return;
    }

    if(FileOrDir){
        // open one file
        gtk_file_dialog_set_title(FileDia, "Choose Files");
        gtk_file_dialog_open (FileDia, ParentWindow->Window, NULL, FileSelected, NULL);
    }else{
        // open one folder
        gtk_file_dialog_set_title(FileDia, "Choose Folder");
        gtk_file_dialog_select_folder(FileDia, ParentWindow->Window, NULL, FolderSelected, NULL);
    }
}

void FileSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = NULL;
    File = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == NULL) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }
    OpenFile(*File, nullptr);
}


void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = NULL;
    File = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == NULL) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }

    ReadAsRootFoler(*File);
}

void ReadAsRootFoler(GFile &folder){
    shared_ptr<FPFolderButton> NewFolder = NewFolderButton();
    NewFolder->init(folder, NULL ,0);
    NewFolder->SetAsRoot(ParentWindow->FP->FileTree);
}

void ReadFolder(GFile &folder, FPFolderButton &F){

    GFileEnumerator *FileEnum = g_file_enumerate_children(&folder, "", GFileQueryInfoFlags::G_FILE_QUERY_INFO_NONE, NULL, NULL);

    while (true) {
        GFileInfo *info = g_file_enumerator_next_file(FileEnum, NULL,NULL);
        if(info == NULL){
            // All file read
            return;
        }

        GFile *fi = g_file_enumerator_get_child(FileEnum,info);
        if(g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY){
            FPFolderButton Child = ParentWindow->FP->NewFolder(fi, &folder, F);
        }else if(g_file_info_get_file_type(info) == G_FILE_TYPE_REGULAR){
            FPFileButton file = ParentWindow->FP->NewFile(fi,F);
        }
    }
}

void OpenFile(GFile &file, FPFileButton* f){
    GtkBuilder *b = gtk_builder_new_from_file("UI/FilePanel.ui");
    shared_ptr<EditArea> ea;
    ea = GetEditAreaFromFileAbsoPath(g_file_get_path(&file));

    if(ea == NULL){
        // Not Opened
        ea = make_shared<EditArea>(&file, f);
        ea->CorreFileButton = f;
        AddEditArea(ea);

        //gtk_stack_add_child(ParentWindow->EAHolder->Container, GTK_WIDGET(ea->BaseGrid));
    }else{
        // Opened
    }
    ParentWindow->EAHolder->Show(ea);

    ea->UnrefBuilder();
}

void CreateFile(EditArea &ea){
    gtk_file_dialog_save(FileDia, GetAppWindow().Window, NULL, FileCreated, &ea);
}

void FileCreated(GObject *source, GAsyncResult *result, void *data){
    EditArea* ea = (EditArea*)data;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, NULL);
    ea->EditingFile = file;
    ea->Save();
    ea->LoadFile(file);

}
