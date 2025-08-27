#include "FileManager.h"

#include <cstddef>
#include <cstdio>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>
#include <string>
#include <vector>

#include "EditArea.h"
#include "FilePanel.h"
#include "Core.h"
#include "MainWindow.h"
#include "ToolFunctions.h"

GtkFileDialog *FileDia;

using namespace std;

void InitFileManager(MainWindow *parent){
    FileDia = gtk_file_dialog_new();
}

void OpenFileChooser(bool FileOrDir){
    // Pass True if select files. Pass False if select folder
    if(GetAppWindow().Window == nullptr){
        g_print("Use 'SetParentWindow()' to set the parent first");
        return;
    }

    if(FileOrDir){
        // open one file
        gtk_file_dialog_set_title(FileDia, "Choose Files");
        gtk_file_dialog_open (FileDia, GetAppWindow().Window, nullptr, FileSelected, nullptr);
    }else{
        // open one folder
        gtk_file_dialog_set_title(FileDia, "Choose Folder");
        gtk_file_dialog_select_folder(FileDia, GetAppWindow().Window, nullptr, FolderSelected, nullptr);
    }
}

void FileSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = nullptr;
    File = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == nullptr) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }
    OpenFile(File, nullptr);
}


void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *File;
    GError *err = nullptr;
    File = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(File == nullptr) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }

    ReadAsRootFolder(*File);
}

void ReadAsRootFolder(GFile &folder){
    shared_ptr<FPFolderButton> &NewFolder = NewFolderButton();
    NewFolder->init(folder, nullptr ,0);
    GetAppWindow().FP->AddNewRootFolder(*NewFolder.get());
}

void ReadFolder(GFile *folder, FPFolderButton &folderbutton){
    GFileEnumerator *FileEnum = g_file_enumerate_children(folder, "standard::name,standard::type", GFileQueryInfoFlags::G_FILE_QUERY_INFO_NONE, nullptr, nullptr);

    if (FileEnum == nullptr) {
        g_print("Failed to enumerate folder contents\n");
        return;
    }


    GFileInfo *info;
    while ((info = g_file_enumerator_next_file(FileEnum, nullptr, nullptr)) != nullptr) {
        GFile *child = g_file_enumerator_get_child(FileEnum, info);
        if (child == nullptr) continue;

        if (g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY) {
            shared_ptr<FPFolderButton> &childfolderbutton = NewFolderButton();
            childfolderbutton->init(*child, folder, folderbutton.Level + 1);
            folderbutton.AddChildFolder(*childfolderbutton.get());
        } else if (g_file_info_get_file_type(info) == G_FILE_TYPE_REGULAR) {
            shared_ptr<FPFileButton> &childfilebutton = NewFileButton();
            childfilebutton->init(child, folderbutton.Level + 1);
            folderbutton.AddChildFile(*childfilebutton.get());
        }

        //g_object_unref(child);=> child (pointer) will be given to FPFolderButton and FPFileButton so it is moved to destructor of the Buttons
        g_object_unref(info);
    }

    g_object_unref(FileEnum);
}

void OpenFile(GFile *file, FPFileButton* f){
    GtkBuilder *b = gtk_builder_new_from_file("UI/FilePanel.ui");
    shared_ptr<EditArea> *ea = GetEditAreaFromFileAbsoPath(g_file_get_path(file));

    if(ea == nullptr){
        // Not Opened before
        ea = &NewEditArea(file, f);
    }else{
        // Opened before
    }
    GetAppWindow().EAHolder->Show(*ea);

    (*ea)->UnrefBuilder();
}

void CreateFile(EditArea &ea){
    gtk_file_dialog_save(FileDia, GetAppWindow().Window, nullptr, FileCreated, &ea);
}

void FileCreated(GObject *source, GAsyncResult *result, void *data){
    EditArea* ea = (EditArea*)data;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);
    ea->EditingFile = file;
    ea->Save();
    ea->LoadFile(file);
}
