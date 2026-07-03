#include "FileOperation.h"

#include "datatypes/file.h"
#include "FileCallback.h"
#include "FileReader.h"
#include "src/session/EditAreaData.h"
#include "src/session/FileData.h"
#include "Gui_if.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"
#include "windows/MainWindow.h"
#include "toolset/tools/Tool.h"

#include <gio/gio.h>
#include <gtk/gtk.h>

static GtkFileDialog *file_dialog;

typedef void (*LocationChoosenCallback)(FileData*);

typedef struct {
    const char* content;
    void (*fileSavedCallback)(FileData*);
}SaveFileUserData;


static void WriteFile(FileData* filedata, const char* content){
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
}

static void OnFileDialogFileSelected(GObject *source, GAsyncResult *result, void *data){
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

    const SimpleEvent& event = filemanager::GetEvent(filemanager::FILE_EVENT_FILE_CHOOSEN);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LocationChoosenCallback)callback)(filedata);
    }
    session::AddSingleFile(filedata);
    session::EditFile(filedata);
}

static void OnFileDialogFolderSelected(GObject *source, GAsyncResult *result, void *data){
    GFile *file;
    GFileInfo *info;
    GError *err = nullptr;

    file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }
    info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    FileData* filedata = filemanager::LoadFileData(file, info, false);

    const SimpleEvent &event = GetEvent(filemanager::FILE_EVENT_FOLDER_CHOOSEN);
    for (EventCallback callback : event.GetCallbackSet()){
        ((LocationChoosenCallback)callback)(filedata);
    }
    session::NewWorkspace(filedata);
}

static void OnFileSaved(GObject *source, GAsyncResult *result, void* data){
    SaveFileUserData* userdata = (SaveFileUserData*) data;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

    if(file != nullptr){
        GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
        FileData* filedata = filemanager::LoadFileData(file, info, true);
        WriteFile(filedata, userdata->content);
        session::AddSingleFile(filedata);
        if(userdata->fileSavedCallback){
            userdata->fileSavedCallback(filedata);
        }
    }

    delete userdata;
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

void SaveFile(FileData* filedata, const char *content, void (*savedcallback)(FileData*)){
    if(filedata->isVirtual){
        SaveFileUserData* userdata = new SaveFileUserData();// free by OnFileSaved()
        userdata->fileSavedCallback = savedcallback;
        userdata->content = content;
        gtk_file_dialog_set_title(file_dialog, "Save file");
        gtk_file_dialog_save(file_dialog, gui::GetMainWindow()->GetGtkWindow(), nullptr, OnFileSaved, userdata);
    } else {
        WriteFile(filedata, content);
    }
}



void ChooseFile(){
    gtk_file_dialog_set_title(file_dialog, "choose a file");
    gtk_file_dialog_open(file_dialog, gui::GetMainWindow()->GetGtkWindow(), nullptr, OnFileDialogFileSelected, nullptr);
}

void ChooseFolder(){
    gtk_file_dialog_set_title(file_dialog, "Choose a folder");
    gtk_file_dialog_select_folder(file_dialog, gui::GetMainWindow()->GetGtkWindow(), nullptr, OnFileDialogFolderSelected, nullptr);
}

}// namespace filemanager
