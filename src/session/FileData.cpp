#include "FileData.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileManagement_if.h"
#include "src/session/SessionEvent.h"
#include "toolset/event/Event.h"
#include "toolset/tools//Tool.h"


/*
 * Typedef
 */
typedef void (*NewWorkspaceCallback)(Workspace*);

static std::unordered_set<Workspace*> workspace_list;

namespace session {

Workspace* NewWorkspace(FileData* rootfolderdata){
    if(rootfolderdata->type != G_FILE_TYPE_DIRECTORY) return nullptr;
    Workspace* ws = new Workspace(rootfolderdata);
    //ws->wsBranch = filemanager::CreateFileTree(rootfolderdata);
    workspace_list.emplace(ws);
    SimpleEvent &event = GetEvent(NEW_WORKSPACE);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((NewWorkspaceCallback)callback)(ws);
    }
    return ws;
}

const std::unordered_set<Workspace*> &GetWorkspaceList(){
    return workspace_list;
}

Workspace* FindWorkspaceByFileData(FileData *filedata){
    for(Workspace* ws : workspace_list){
        if(tools::StartWith(filedata->absoPath, ws->GetFileData()->absoPath)){
            return ws;
        }
    }

    return nullptr;
}

Workspace* FindWorkspaceByPath(const char* path){
    for(Workspace* ws : workspace_list){
        if(tools::StartWith(path, ws->GetFileData()->absoPath)){
            return ws;
        }
    }

    return nullptr;
}

}// namespace session
