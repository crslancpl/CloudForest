#ifndef FILECALLBACK_H_
#define FILECALLBACK_H_

#include "datatypes/common.h"

#include <gio/gio.h>

namespace filemanagement{

enum Event : char{
    FILE_EVENT_FILE_CHOOSEN,
    FILE_EVENT_FOLDER_CHOOSEN,
    FILE_EVENT_FILE_SAVED
};

void ListenEvent(filemanagement::Event event, EventCallback callback);
void StopListenEvent(filemanagement::Event event, EventCallback callback);

void RunCallback(Event event, GFile* file, GFileInfo* fileinfo);

}



#endif
