#include "FileOperation.h"

#include "FileCallback.h"
#include "FileOperation.h"
#include "datatypes/common.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"
#include "src/gui/Gui_if.h"
#include "src/gui/editarea/EditArea.h"
#include "src/gui/editarea/EditArea_if.h"
#include "src/gui/windows/MainWindow.h"
#include "toolset/tools/Tool.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <unordered_map>
#include <unordered_set>

static GtkFileDialog *file_dialog;
static char *file_content;
static void (*file_saved_callback)(FileData*);
static std::unordered_map<FileData*, EditArea*> file_editarea_map;
static std::unordered_set<Workspace*> workspace_list;

static void FileSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *info;
    GError *err = nullptr;
    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }
    info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    FileData* filedata = filemanagement::LoadFileData(file, info);
    filemanagement::OpenFile(filedata);
}

static void FolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *info;
    GError *err = nullptr;
    file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }
    info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    FileData* folderdata = filemanagement::LoadFileData(file, info);
    filemanagement::NewWorkspace(folderdata);
}

static void FileSaved(GObject *source, GAsyncResult *result, void* content){
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

    if(file != nullptr){
        GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
        FileData* filedata = filemanagement::LoadFileData(file, info);
        filemanagement::SaveFile(filedata, file_content, file_saved_callback);
    }else{
        //Callback(nullptr, nullptr);
    }
}


void FileOperationInit(){
    file_dialog = gtk_file_dialog_new();
}

/*
void filemanagement::CreateFile(GFile* file){
    gtk_file_dialog_set_title(file_dialog, "Save file");
    gtk_file_dialog_save(file_dialog, gui::g_mainwindow->m_window, nullptr, FileSaved, nullptr);
}
*/

FileData* filemanagement::CreateVirtualFile(){
    FileData* data = new FileData();
    data->fileName = strdup("untitled");
    data->absoPath = strdup(("virtual/" + tools::GenerateId()).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;
    return data;
}


void filemanagement::SaveFile(FileData* filedata, char *content, void (*savedcallback)(FileData*)){
    if(filedata->isVirtual){
        file_content = content;
        file_saved_callback = savedcallback;
        gtk_file_dialog_set_title(file_dialog, "Save file");
        gtk_file_dialog_save(file_dialog, gui::g_mainwindow->GetGtkWindow(), nullptr, FileSaved, nullptr);
        return;
    }

    g_file_replace_contents(
        filedata->file,
        content,
        strlen(content),
        nullptr,
        false,
        GFileCreateFlags::G_FILE_CREATE_REPLACE_DESTINATION,
        nullptr,
        nullptr,
        nullptr);

    if(savedcallback){
        savedcallback(filedata);
    }
}

void filemanagement::ChooseFile(){
    gtk_file_dialog_set_title(file_dialog, "choose a file");
    gtk_file_dialog_open(file_dialog, gui::g_mainwindow->GetGtkWindow(), nullptr, FileSelected, nullptr);
}

void filemanagement::ChooseFolder(){
    gtk_file_dialog_set_title(file_dialog, "Choose a folder");
    gtk_file_dialog_select_folder(file_dialog, gui::g_mainwindow->GetGtkWindow(), nullptr, FolderSelected, nullptr);
}

void filemanagement::NewWorkspace(FileData* rootfolderdata){
    if(rootfolderdata->type != G_FILE_TYPE_DIRECTORY) return;
    Workspace* ws = new Workspace();
    ws->name = rootfolderdata->fileName;
    ws->rootFolderData = rootfolderdata;
    ws->wsBranch = CreateFileTree(rootfolderdata);
    workspace_list.emplace(ws);
    InvokeNewWorkspace(ws);
}

const std::unordered_set<Workspace*> &filemanagement::GetWorkspaceList(){
    return workspace_list;
}

Workspace* filemanagement::FindWorkspace(FileData *filedata){
    for(Workspace* ws : workspace_list){
        if(tools::StartWith(filedata->absoPath, ws->rootFolderData->absoPath)){
            return ws;
        }
    }

    return nullptr;
}

Workspace* filemanagement::FindWorkspaceFromPath(const char* path){
    for(Workspace* ws : workspace_list){
        if(tools::StartWith(path, ws->rootFolderData->absoPath)){
            return ws;
        }
    }

    return nullptr;
}

void filemanagement::OpenFile(FileData *filedata){
    for(Workspace* ws : workspace_list){
        if(tools::StartWith(filedata->absoPath, ws->rootFolderData->absoPath)){

        }
    }
    editarea::OpenFile(filedata);
}

void filemanagement::CloseFile(FileData *filedata){
    editarea::CloseFile(filedata);
}
