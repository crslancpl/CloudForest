#ifndef FILECALLBACK_H_
#define FILECALLBACK_H_

#include "toolset/event/Event.h"

#include <gio/gio.h>

// forward declare
typedef struct Workspace Workspace;

namespace filemanager{

enum Signal : char{
    FILE_EVENT_FILE_CHOOSEN,
    FILE_EVENT_FOLDER_CHOOSEN,
    FILE_EVENT_FILE_SAVED
};

void Listen(Signal event, EventCallback callback);
void StopListen(Signal event, EventCallback callback);

const SimpleEvent& GetEvent(Signal signal);

}



#endif
