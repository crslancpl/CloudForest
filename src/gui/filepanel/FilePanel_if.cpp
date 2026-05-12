#include "FilePanel_if.h"

#include "FilePanel.h"
#include "datatypes/common.h"
#include "datatypes/file.h"
#include "../Gui_if.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileManagement_if.h"


void filepanel::AddWorkspaceBox(Workspace* ws){
    /*
     * Load a folder as root (Level = 0)
     */

    gui::g_filepanel->NewWorkspace(ws);
}

void filepanel::Construct(){
    if(!gui::g_filepanel){
        gui::g_filepanel = new FilePanel();
        filemanagement::ListenEvent(filemanagement::FILE_EVENT_NEW_WORKSPACE, (EventCallback)filepanel::AddWorkspaceBox);
        /*
        filemanagement::ListenEvent(filemanagement::FILE_EVENT_FOLDER_CHOOSEN, (EventCallback)filepanel::LoadAsRoot);
        filemanagement::ListenEvent(filemanagement::FILE_EVENT_FILE_CHOOSEN, (EventCallback)filepanel::LoadAsRoot);
        */
    }
}
