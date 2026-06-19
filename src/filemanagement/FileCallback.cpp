#include "FileCallback.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/filemanagement/FileOperation.h"
#include "toolset/event/Event.h"

#include <gio/gio.h>
#include <unordered_map>

typedef void(*FileChoosenCallback)(FileData*);
typedef void(*FolderChoosenCallback)(FileData*);
typedef void(*NewWorkspaceCallback)(const Workspace*) ;

namespace filemanager{

static std::unordered_map<Signal, SimpleEvent> event_map = {
    {FILE_EVENT_FILE_CHOOSEN, SimpleEvent()},
    {FILE_EVENT_FOLDER_CHOOSEN, SimpleEvent()}
};

void Listen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void StopListen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}

void InvokeFileChoosen(FileData* file){
    const SimpleEvent &event = event_map.at(FILE_EVENT_FILE_CHOOSEN);
    for(EventCallback callback : event.GetCallbackSet()){
        ((FileChoosenCallback)callback)(file);
    }
}

void InvokeFolderChoosen(FileData* file){
    const SimpleEvent &event = event_map.at(FILE_EVENT_FOLDER_CHOOSEN);
    for (EventCallback callback : event.GetCallbackSet()){
        ((FolderChoosenCallback)callback)(file);
    }
}

}// namespace filemanager
