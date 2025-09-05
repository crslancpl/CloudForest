#include "FileManager.h"

#include <cstddef>
#include <cstdio>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>
#include <string>
#include <vector>

#include "Core.h"
#include "gui/guiCore.h"
#include "ToolFunctions.h"

static GtkFileDialog *FileDia;

static void (*Callback)(GFile *file, GFileInfo *fileinfo);

static void FileSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *fileinfo;
    GError *err = nullptr;
    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }
    fileinfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    Callback(file, fileinfo);
}

static void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *fileinfo;
    GError *err = nullptr;
    file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        g_print("Cancelled\n");
        return;
    }
    fileinfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    Callback(file, fileinfo);
}

void filemanag::Init(){
    FileDia = gtk_file_dialog_new();
}

void filemanag::Process(FileAction* action){
    Callback = action->callback;
    if(action->actiontype == FileAction::OPENFILE){
        OpenFileChooser(true);
    }else if(action->actiontype == FileAction::OPENFOLDER){
        OpenFileChooser(false);
    }else if(action->actiontype == FileAction::ENUMERATE){
        EnumerateFolderChild((GFile*)action->data);
    }
}

void filemanag::OpenFileChooser(bool FileOrDir){
    // Pass True if select files. Pass False if select folder

    if(FileOrDir){
        // open one file
        gtk_file_dialog_set_title(FileDia, "Choose Files");
        gtk_file_dialog_open (FileDia, gui::AppWindow.Window, nullptr, FileSelected, nullptr);
    }else{
        // open one folder
        gtk_file_dialog_set_title(FileDia, "Choose Folder");
        gtk_file_dialog_select_folder(FileDia, gui::AppWindow.Window, nullptr, FolderSelected, nullptr);
    }
}

void filemanag::LoadText(GFile* file,char* &textoutput){
    /*
     * load the content to textoutput
     */
     g_file_load_contents(file, nullptr, &textoutput, nullptr, nullptr, nullptr);
}

void filemanag::EnumerateFolderChild(GFile* folder){
    /*
     * Enumerate children in folder and call the callback function
     * every child file/folder. Callback nullptr and nullptr when the
     * children are all enumerated.
     */

    GFileEnumerator *enumerator = g_file_enumerate_children(folder, "*", G_FILE_QUERY_INFO_NONE,
        nullptr, nullptr);

    GFileInfo *info;
    while ((info = g_file_enumerator_next_file(enumerator, nullptr, nullptr)) != nullptr) {
        GFile *child = g_file_enumerator_get_child(enumerator, info);
        if (child == nullptr) continue;
        Callback(child, info);
    }

    Callback(nullptr, nullptr);
}
