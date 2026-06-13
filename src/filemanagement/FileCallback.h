#ifndef FILECALLBACK_H_
#define FILECALLBACK_H_

#include "toolset/event/Event.h"
#include "src/filemanagement/FileOperation.h"

#include <gio/gio.h>

// forward declare
typedef struct Workspace Workspace;

namespace filemanagement{

enum Signal : char{
    FILE_EVENT_FILE_CHOOSEN,
    FILE_EVENT_FOLDER_CHOOSEN,
    FILE_EVENT_FILE_SAVED,
    FILE_EVENT_NEW_WORKSPACE
};

void Listen(Signal event, EventCallback callback);
void StopListen(Signal event, EventCallback callback);

void InvokeFileChoosen(FileData* file);
void InvokeFolderChoosen(FileData* file);
void InvokeNewWorkspace(const Workspace* ws);

}



#endif
