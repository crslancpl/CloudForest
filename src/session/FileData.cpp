#include "FileData.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/session/SessionEvent.h"
#include "toolset/event/Event.h"
#include "toolset/tools//Tool.h"

#include <unordered_map>
/*
 * Typedef
 */
typedef void (*NewWorkspaceCallback)(Workspace*);

typedef struct {
    std::unordered_set<Workspace*> workspaceList;
    std::unordered_map<std::string, FileData*> singleFileDataMap;
}SessionFileData;

SessionFileData session_file_data;


namespace session {

Workspace* NewWorkspace(FileData* rootfolderdata){
    if(rootfolderdata->type != G_FILE_TYPE_DIRECTORY) return nullptr;
    Workspace* ws = new Workspace(rootfolderdata);
    //ws->wsBranch = filemanager::CreateFileTree(rootfolderdata);
    session_file_data.workspaceList.emplace(ws);
    SimpleEvent &event = GetEvent(NEW_WORKSPACE);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((NewWorkspaceCallback)callback)(ws);
    }
    return ws;
}

const std::unordered_set<Workspace*> &GetWorkspaceList(){
    return session_file_data.workspaceList;
}

Workspace* FindWorkspaceByFileData(FileData *filedata){
    for(Workspace* ws : session_file_data.workspaceList){
        if(tools::StartWith(filedata->absoPath, ws->GetFileData()->absoPath)){
            return ws;
        }
    }

    return nullptr;
}

Workspace* FindWorkspaceByPath(const char* path){
    for(Workspace* ws : session_file_data.workspaceList){
        if(tools::StartWith(path, ws->GetFileData()->absoPath)){
            return ws;
        }
    }

    return nullptr;
}


void AddSingleFile(FileData* filedata){
    session_file_data.singleFileDataMap.emplace(filedata->absoPath, filedata);
}

void RemoveSingleFile(FileData *filedata){
    session_file_data.singleFileDataMap.erase(filedata->absoPath);
}

FileData* FindSingleFileByPath(const char* path){
    auto itr = session_file_data.singleFileDataMap.find(path);
    if (itr != session_file_data.singleFileDataMap.end()) {
        return itr->second;
    }
    return nullptr;
}

}// namespace session
