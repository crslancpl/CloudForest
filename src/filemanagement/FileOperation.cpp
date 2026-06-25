#include "FileOperation.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"
#include "src/session/EditAreaData.h"
#include "src/session/FileData.h"
#include "Gui_if.h"
#include "editarea/EditArea.h"
#include "windows/MainWindow.h"
#include "toolset/tools/Tool.h"

#include <filesystem>
#include <gio/gio.h>
#include <gtk/gtk.h>

static GtkFileDialog *file_dialog;
static char *file_content;
static void (*file_saved_callback)(FileData*);


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
    FileData* filedata = filemanager::LoadFileData(file, info, true);
    session::AddSingleFile(filedata);
    session::EditFile(filedata);
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
    FileData* folderdata = filemanager::LoadFileData(file, info, false);
    session::NewWorkspace(folderdata);
}

static void FileSaved(GObject *source, GAsyncResult *result, void* content){
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

    if(file != nullptr){
        GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
        FileData* filedata = filemanager::LoadFileData(file, info, true);
        filemanager::SaveFile(filedata, file_content, file_saved_callback);
    }else{
        //Callback(nullptr, nullptr);
    }
}


void FileOperationInit(){
    file_dialog = gtk_file_dialog_new();
}

namespace filemanager{

FileData* CreateNewFile(){
    FileData* data = new FileData();
    std::string name = "untitled" + tools::GenerateId();
    data->fileName = strdup(name.c_str());
    data->absoPath = strdup(("_/" + name).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;
    return data;
}

FileData* CreateVirtualFile(){
    FileData* data = new FileData();
    std::string name = "untitled" + tools::GenerateId();
    data->fileName = strdup(name.c_str());
    data->absoPath = strdup(("virtual/" + name).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;
    return data;
}

FileData* CreateVirtualFolder(){
    FileData* data = new FileData();
    data->fileName = strdup("virtual");
    data->absoPath = strdup("virtual/");
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_DIRECTORY;
    return data;
}

void SaveFile(FileData* filedata, char *content, void (*savedcallback)(FileData*)){
    if(filedata->isVirtual){
        file_content = content;
        file_saved_callback = savedcallback;
        gtk_file_dialog_set_title(file_dialog, "Save file");
        gtk_file_dialog_save(file_dialog, gui::GetMainWindow()->GetGtkWindow(), nullptr, FileSaved, nullptr);
        return;
    }

    g_file_replace_contents(
        filedata->file,
        content,
        strlen(content),
        nullptr,
        false,
        G_FILE_CREATE_REPLACE_DESTINATION,
        nullptr,
        nullptr,
        nullptr);

    if(savedcallback){
        savedcallback(filedata);
    }
}

void ChooseFile(){
    gtk_file_dialog_set_title(file_dialog, "choose a file");
    gtk_file_dialog_open(file_dialog, gui::GetMainWindow()->GetGtkWindow(), nullptr, FileSelected, nullptr);
}

void ChooseFolder(){
    gtk_file_dialog_set_title(file_dialog, "Choose a folder");
    gtk_file_dialog_select_folder(file_dialog, gui::GetMainWindow()->GetGtkWindow(), nullptr, FolderSelected, nullptr);
}

}// namespace filemanager
