#include "FileCallback.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/filemanagement/FileOperation.h"

#include <gio/gio.h>
#include <unordered_set>

typedef void(*FileChoosenCallback)(FileData*);
typedef void(*FolderChoosenCallback)(FileData*);
typedef void(*NewWorkspaceCallback)(const Workspace*) ;

static std::unordered_set<FileChoosenCallback> file_choosen_callbacks;
static std::unordered_set<FolderChoosenCallback> folder_choosen_callbacks;
static std::unordered_set<NewWorkspaceCallback> new_workspace_callbacks;

void filemanagement::ListenEvent(filemanagement::Event event, EventCallback callback){
    switch (event) {
    case FILE_EVENT_FILE_CHOOSEN:
        file_choosen_callbacks.emplace((FileChoosenCallback)callback);
        break;
    case FILE_EVENT_FOLDER_CHOOSEN:
        folder_choosen_callbacks.emplace((FolderChoosenCallback)callback);
        break;
    case FILE_EVENT_NEW_WORKSPACE:
        new_workspace_callbacks.emplace((NewWorkspaceCallback)callback);
        break;
    default:
        break;
    }
}

void filemanagement::StopListenEvent(filemanagement::Event event, EventCallback callback){
    switch (event) {
    case FILE_EVENT_FILE_CHOOSEN:
        file_choosen_callbacks.erase((FileChoosenCallback)callback);
        break;
    case FILE_EVENT_FOLDER_CHOOSEN:
        folder_choosen_callbacks.erase((FolderChoosenCallback)callback);
        break;
    case FILE_EVENT_NEW_WORKSPACE:
        new_workspace_callbacks.erase((NewWorkspaceCallback)callback);
        break;
    default:
        break;
    }
}

void filemanagement::InvokeFileChoosen(FileData* file){
    for (FileChoosenCallback cb: file_choosen_callbacks){
        cb(file);
    }
}
void filemanagement::InvokeFolderChoosen(FileData* file){
    for (FolderChoosenCallback cb: folder_choosen_callbacks){
        cb(file);
    }
}
void filemanagement::InvokeNewWorkspace(const Workspace* ws){
    for (NewWorkspaceCallback cb: new_workspace_callbacks){
        cb(ws);
    }
}
