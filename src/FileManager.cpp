#include "FileManager.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
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
        return;
    }
    fileinfo = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    Callback(file, fileinfo);
}

void filemanag::Init(){
    /*
     * Create a file dialog for the future
     */
    FileDia = gtk_file_dialog_new();
}

void filemanag::Process(request::Request* request){
    if(auto req = dynamic_cast<request::FileOpenFile*>(request)){
        Callback = req->Callback;
        OpenFileChooser(true);
    }else if(auto req = dynamic_cast<request::FileOpenFolder*>(request)){
        Callback = req->Callback;
        OpenFileChooser(false);
    }else if(auto req = dynamic_cast<request::FileEnumerate*>(request)){
        Callback = req->Callback;
        EnumerateFolderChild(req->File);
    }else if(auto req = dynamic_cast<request::FileSave*>(request)){
        Callback = req->Callback;
        Save(req->File, &req->Content);
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

static void FileCreated(GObject *source, GAsyncResult *result, void* content){
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

    if(file != nullptr){
        filemanag::Save(file, nullptr);
    }else{
        Callback(nullptr, nullptr);
    }
}

static void OpenFileSaver(){
    gtk_file_dialog_save(FileDia, gui::AppWindow.Window, nullptr, FileCreated,nullptr);
}

void filemanag::Save(GFile *file, std::string* content){
    static std::string contentcache;
    if(file == nullptr){
        contentcache = *content;
        OpenFileSaver();
    }else{
        if(content != nullptr){
            // the file is already exist so we don't have to choose the gfile
            g_file_replace_contents(
                file, content->c_str(), content->size(),
                nullptr, false, GFileCreateFlags::G_FILE_CREATE_REPLACE_DESTINATION, nullptr, nullptr, nullptr);
        }else{
            // the file is newly created
            g_file_replace_contents(
                file, contentcache.c_str(), contentcache.size(),
                nullptr, false, GFileCreateFlags::G_FILE_CREATE_REPLACE_DESTINATION, nullptr, nullptr, nullptr);
        }
        Callback(file, g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr));
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
