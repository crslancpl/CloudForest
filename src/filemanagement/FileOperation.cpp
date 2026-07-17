#include "FileOperation.h"

#include "datatypes/file.h"
#include "FileCallback.h"
#include "FileReader.h"
#include "src/App.h"
#include "src/session/EditAreaData.h"
#include "src/session/FileData.h"
#include "Gui_if.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"
#include "windows/MainWindow.h"
#include "toolset/tools/Tool.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>


// typedef
typedef void (*LocationChoosenCallback)(FileData*);

typedef struct {
    FileData* oldFileData;
    const char* content;
    void (*fileSavedCallback)(FileData*);
}SaveFileUserData;


// static variable
static App* current_app = nullptr;



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
    std::unique_ptr<FileData> filedata = filemanager::LoadFileData(file, info, true);
    FileData* filedataraw = filedata.get();

    const SimpleEvent& event = filemanager::GetEvent(filemanager::FILE_EVENT_FILE_CHOOSEN);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LocationChoosenCallback)callback)(filedataraw);
    }
    session::AddSingleFile(std::move(filedata));
    session::EditFile(filedataraw);
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
    std::unique_ptr<FileData> filedata = filemanager::LoadFileData(file, info, false);
    FileData* filedataraw = filedata.get();

    const SimpleEvent &event = GetEvent(filemanager::FILE_EVENT_FOLDER_CHOOSEN);
    for (EventCallback callback : event.GetCallbackSet()){
        ((LocationChoosenCallback)callback)(filedataraw);
    }
    session::NewWorkspace(std::move(filedata));
}

static void OnFileSaved(GObject *source, GAsyncResult *result, void* data){
    SaveFileUserData* userdata = (SaveFileUserData*) data;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);

    if(file != nullptr){
        session::RemoveSingleFile(userdata->oldFileData);

        GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
        std::unique_ptr<FileData> filedata = filemanager::LoadFileData(file, info, true);

        FileData* filedataraw = filedata.get();
        WriteFile(filedata.get(), userdata->content);
        session::AddSingleFile(std::move(filedata));
        if(userdata->fileSavedCallback){
            userdata->fileSavedCallback(filedataraw);
        }
    }

    delete userdata;
}

void FileOperationInit(App& app){
    current_app = &app;
    current_app->appUI.fileDialog = gtk_file_dialog_new();
}

namespace filemanager{

std::unique_ptr<FileData> CreateNewFile(){
    std::unique_ptr<FileData> data = std::make_unique<FileData>();
    std::string name = "untitled" + tools::GenerateId();
    data->fileName = strdup(name.c_str());
    data->absoPath = strdup(("_/" + name).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;
    return data;
}

std::unique_ptr<FileData> CreateVirtualFile(){
    std::unique_ptr<FileData> data = std::make_unique<FileData>();
    std::string name = "untitled" + tools::GenerateId();
    data->fileName = strdup(name.c_str());
    data->absoPath = strdup(("virtual/" + name).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;
    return data;
}

std::unique_ptr<FileData> CreateVirtualFolder(){
    std::unique_ptr<FileData> data = std::make_unique<FileData>();
    data->fileName = strdup("virtual");
    data->absoPath = strdup("virtual/");
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_DIRECTORY;
    return data;
}

void SaveFile(FileData* filedata, const char *content, void (*savedcallback)(FileData*)){
    if(filedata->isVirtual){
        SaveFileUserData* userdata = new SaveFileUserData{
            filedata,
            content,
            savedcallback
        };// free by OnFileSaved()

        gtk_file_dialog_set_title(current_app->appUI.fileDialog, "Save file");
        GtkWindow* parentGtkWindow = current_app->appUI.mainWindow->GetGtkWindow();
        gtk_file_dialog_save(current_app->appUI.fileDialog, parentGtkWindow, nullptr, OnFileSaved, userdata);
    } else {
        WriteFile(filedata, content);
    }
}



void ChooseFile(){
    gtk_file_dialog_set_title(current_app->appUI.fileDialog, "choose a file");
    if (current_app == nullptr){
        return;
    }
    GtkWindow* parentGtkWindow = current_app->appUI.mainWindow->GetGtkWindow();
    gtk_file_dialog_open(current_app->appUI.fileDialog, parentGtkWindow, nullptr, OnFileDialogFileSelected, nullptr);
}

void ChooseFolder(){
    gtk_file_dialog_set_title(current_app->appUI.fileDialog, "Choose a folder");
    GtkWindow* parentGtkWindow = current_app->appUI.mainWindow->GetGtkWindow();
    gtk_file_dialog_select_folder(current_app->appUI.fileDialog, parentGtkWindow, nullptr, OnFileDialogFolderSelected, nullptr);
}

}// namespace filemanager
