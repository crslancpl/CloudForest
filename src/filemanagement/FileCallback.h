#ifndef FILECALLBACK_H_
#define FILECALLBACK_H_

#include "src/filemanagement/FileManagement_if.h"
#include <gio/gio.h>

void RunCallback(filemanagement::Event event, GFile* file, GFileInfo* fileinfo);

#endif
