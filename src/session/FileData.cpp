#include "FileData.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileCallback.h"
#include "src/session/SessionEvent.h"
#include "toolset/event/Event.h"
#include "toolset/tools//Tool.h"

#include <memory>
#include <unordered_map>
/*
 * Typedef
 */
typedef void (*NewWorkspaceCallback)(Workspace*);

typedef struct {
    std::unordered_set<std::unique_ptr<Workspace>> workspaceList;
    std::unordered_map<std::string, std::unique_ptr<FileData>> singleFileDataMap;
}SessionFileData;

SessionFileData session_file_data;


namespace session {

Workspace* NewWorkspace(std::unique_ptr<FileData> rootfolderdata){
    if(rootfolderdata->type != G_FILE_TYPE_DIRECTORY) return nullptr;
    std::unique_ptr<Workspace> ws = std::make_unique<Workspace>(std::move(rootfolderdata));
    Workspace* wsraw = ws.get();
    session_file_data.workspaceList.emplace(std::move(ws));
    SimpleEvent &event = GetEvent(NEW_WORKSPACE);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((NewWorkspaceCallback)callback)(wsraw);
    }
    return wsraw;
}

const std::unordered_set<std::unique_ptr<Workspace>> &GetWorkspaceList(){
    return session_file_data.workspaceList;
}

Workspace* FindWorkspaceByFileData(FileData *filedata){
    for(const std::unique_ptr<Workspace>& ws : session_file_data.workspaceList){
        if(tools::StartWith(filedata->absoPath, ws->GetFileData()->absoPath)){
            return ws.get();
        }
    }

    return nullptr;
}

Workspace* FindWorkspaceByPath(const char* path){
    for(const std::unique_ptr<Workspace>& ws : session_file_data.workspaceList){
        if(tools::StartWith(path, ws->GetFileData()->absoPath)){
            return ws.get();
        }
    }

    return nullptr;
}


void AddSingleFile(std::unique_ptr<FileData> filedata){
    session_file_data.singleFileDataMap.emplace(filedata->absoPath, std::move(filedata));
}

void RemoveSingleFile(FileData *filedata){
    session_file_data.singleFileDataMap.erase(filedata->absoPath);
}

FileData* FindSingleFileByPath(const char* path){
    auto itr = session_file_data.singleFileDataMap.find(path);
    if (itr != session_file_data.singleFileDataMap.end()) {
        return itr->second.get();
    }
    return nullptr;
}

}// namespace session
