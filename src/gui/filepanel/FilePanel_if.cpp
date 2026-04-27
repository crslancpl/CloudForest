#include "FilePanel_if.h"

#include "FilePanel.h"
#include "datatypes/common.h"
#include "../Gui_if.h"
#include "src/filemanagement/FileManagement_if.h"


void filepanel::LoadAsRoot(GFile *file, GFileInfo *fileinfo){
    /*
     * Load a new file or folder as root (Level = 0)
     */
    GFileType filetype = g_file_info_get_file_type(fileinfo);

    if( filetype == G_FILE_TYPE_DIRECTORY){
        gui::g_filepanel->NewFolder(file, nullptr, nullptr);
    }else if(filetype == G_FILE_TYPE_REGULAR){
        gui::g_filepanel->NewFile(file, nullptr);
    }
}

void filepanel::Construct(){
    if(!gui::g_filepanel){
        gui::g_filepanel = new FilePanel();
        filemanagement::ListenEvent(filemanagement::FILE_EVENT_FOLDER_CHOOSEN, (EventCallback)filepanel::LoadAsRoot);
        filemanagement::ListenEvent(filemanagement::FILE_EVENT_FILE_CHOOSEN, (EventCallback)filepanel::LoadAsRoot);
    }
}
