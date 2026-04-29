#include "FileCallback.h"

#include <gio/gio.h>
#include <unordered_set>

typedef void(*FileChoosenCallback)(GFile*,GFileInfo*);
typedef void(*FolderChoosenCallback)(GFile*,GFileInfo*);

static std::unordered_set<FileChoosenCallback> file_choosen_callbacks;
static std::unordered_set<FolderChoosenCallback> folder_choosen_callbacks;

void filemanagement::ListenEvent(filemanagement::Event event, EventCallback callback){
    switch (event) {
    case FILE_EVENT_FILE_CHOOSEN:
        file_choosen_callbacks.emplace((FileChoosenCallback)callback);
        break;
    case FILE_EVENT_FOLDER_CHOOSEN:
        folder_choosen_callbacks.emplace((FolderChoosenCallback)callback);
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
    default:
        break;
    }
}


void filemanagement::RunCallback(filemanagement::Event event, GFile* file, GFileInfo* fileinfo){
    std::unordered_set<void (*)(GFile*, GFileInfo*)> callbacklist;
    switch (event) {
    case filemanagement::FILE_EVENT_FILE_CHOOSEN:
        callbacklist = file_choosen_callbacks;
        break;
    case filemanagement::FILE_EVENT_FOLDER_CHOOSEN:
        callbacklist = folder_choosen_callbacks;
        break;
    default:
        break;
    }

    for(auto callback : callbacklist){
        callback(file, fileinfo);
    }
}
