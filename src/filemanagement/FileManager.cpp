#include "FileManager.h"

#include "datatypes/file.h"
#include "FileCallback.h"
#include "FileReader.h"
#include "FileTree.h"
#include "filepanel/FilePanel.h"
#include "src/AppContext.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"
#include "windows/MainWindow.h"
#include "toolset/tools/Tool.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>


// typedef
typedef void (*LocationChoosenCallback)(FileData*);


static void on_file_dialog_file_selected(GObject *source, GAsyncResult *result, void *parent){
    GFile *file;
    GError *err = nullptr;

    file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }

    ((FileManager*)parent)->OnNewFileSelected(file);
}

static void on_file_dialog_folder_selected(GObject *source, GAsyncResult *result, void *parent){
    GFile *file;
    GError *err = nullptr;

    file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &err);
    if(file == nullptr) {
        // cancelled
        return;
    }

    ((FileManager*)parent)->OnNewFolderSelected(file);
}




FileManager::FileManager(AppContext &context) :
    m_context(context){
    //
}

FileManager::~FileManager(){}

FileBranch& FileManager::CreateNewFile(){
    std::unique_ptr<FileData> data = std::make_unique<FileData>();
    std::string name = "untitled" + tools::GenerateId();
    data->fileName = strdup(name.c_str());
    data->absoPath = strdup(("_/" + name).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;

    std::unique_ptr<FileBranch> filebranch = std::make_unique<FileBranch>(std::move(data));
    FileBranch& ref = *filebranch;
    m_singleFiles.emplace_back(std::move(filebranch));
    return ref;
}

std::unique_ptr<FileBranch> FileManager::CreateVirtualFile(){
    std::unique_ptr<FileData> data = std::make_unique<FileData>();
    std::string name = "untitled" + tools::GenerateId();
    data->fileName = strdup(name.c_str());
    data->absoPath = strdup(("virtual/" + name).c_str());
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_REGULAR;

    return std::make_unique<FileBranch>(std::move(data));
}

std::unique_ptr<FileBranch> FileManager::CreateVirtualFolder(){
    std::unique_ptr<FileData> data = std::make_unique<FileData>();
    data->fileName = strdup("virtual");
    data->absoPath = strdup("virtual/");
    data->isVirtual = true;
    data->file = nullptr;
    data->type = G_FILE_TYPE_DIRECTORY;

    return std::make_unique<FileBranch>(std::move(data));
}
/*
std::unique_ptr<Workspace> FileManager::CreateWorkspace(){
    //
}
*/
void FileManager::ChooseFile(){
    gtk_file_dialog_set_title(m_context.appUI->GetFileDialog(), "choose a file");
    GtkWindow* parentGtkWindow = m_context.appUI->GetMainWindow()->GetGtkWindow();
    gtk_file_dialog_open(m_context.appUI->GetFileDialog(), parentGtkWindow, nullptr, on_file_dialog_file_selected, this);
}

void FileManager::ChooseFolder(){
    gtk_file_dialog_set_title(m_context.appUI->GetFileDialog(), "Choose a folder");
    GtkWindow* parentGtkWindow = m_context.appUI->GetMainWindow()->GetGtkWindow();
    gtk_file_dialog_select_folder(m_context.appUI->GetFileDialog(), parentGtkWindow, nullptr, on_file_dialog_folder_selected, this);
}

FileBranch* FileManager::FindFileByPath(const std::string& path){
    Workspace* ws = this->FindWorkspaceByPath(path);

    if (ws) {
        return static_cast<FileBranch*>(ws->FindChildBranchByPath(path));
    } else {
        for (std::unique_ptr<FileBranch>& file : m_singleFiles) {
            if (file->GetFileData()->absoPath == path) {
                return file.get();
            }
        }
    }

    return nullptr;
}

Workspace* FileManager::FindWorkspace(const FileBranch& filebranch){
    for(const std::unique_ptr<Workspace>& ws : m_workspaces){
        if(tools::StartWith(filebranch.GetFileData()->absoPath, ws->GetFileData()->absoPath)){
            return ws.get();
        }
    }

    return nullptr;
}

Workspace* FileManager::FindWorkspaceByPath(const std::string& path){
    for(const std::unique_ptr<Workspace>& ws : m_workspaces){
        if(tools::StartWith(path, ws->GetFileData()->absoPath)){
            return ws.get();
        }
    }

    return nullptr;
}

std::vector<std::unique_ptr<Workspace>> &FileManager::GetWorkspaces(){
    return m_workspaces;
}

void FileManager::ConnectWorkspaceCreated(EventReceiver* receiver, void (EventReceiver::*callback)(Workspace*)){
    cb_WorkspaceCreated *newcallback = new cb_WorkspaceCreated();
    newcallback->receiver = receiver;
    newcallback->callback = callback;
    m_workspaceCreatedCbs.emplace_back(newcallback);
}

void FileManager::OnNewFileSelected(GFile* file){
    GError *err;
    GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    std::unique_ptr<FileData> filedata = filemanager::LoadFileData(file, info, true);
    /*
    const SimpleEvent& event = filemanager::GetEvent(filemanager::FILE_EVENT_FILE_CHOOSEN);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LocationChoosenCallback)callback)(filedata.get());
    }
    */
    for (cb_SingleFileOpened* callbackdata : m_singleFileOpenedCbs) {
        EventReceiver* r = callbackdata->receiver;
        void (EventReceiver::*f)(FileData*) = callbackdata->callback;
        (r->*f)(filedata.get());
    }

    std::unique_ptr<FileBranch> branch = std::make_unique<FileBranch>(std::move(filedata));
    FileBranch& b = *branch;

    m_singleFiles.emplace_back(std::move(branch));

    m_context.sessionManager->GetEditorsManager().EditFile(b);
}

void FileManager::OnNewFolderSelected(GFile* file){
    GError *err;
    GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);

    info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, &err);
    std::unique_ptr<FileData> filedata = filemanager::LoadFileData(file, info, false);
    FileData* filedataraw = filedata.get();

    std::unique_ptr<Workspace> workspace = std::make_unique<Workspace>(std::move(filedata));

    for (cb_WorkspaceCreated* callbackdata : m_workspaceCreatedCbs) {
        EventReceiver* r = callbackdata->receiver;
        void (EventReceiver::*f)(Workspace*) = callbackdata->callback;
        (r->*f)(workspace.get());
    }

    m_workspaces.emplace_back(std::move(workspace));
}
