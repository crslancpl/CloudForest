#ifndef FILEMANAGEMENT_IF_H_
#define FILEMANAGEMENT_IF_H_

// include other headers
#include "FileReader.h"
#include "FileOperation.h"
#include "FileCallback.h"


#include <gio/gio.h>
#include <gtk/gtk.h>


//forward definition
typedef struct FileData FileData;


namespace filemanagement{
void Init();
void OpenFile(GFile* file);
void CloseFile(GFile *file);
}


#endif
