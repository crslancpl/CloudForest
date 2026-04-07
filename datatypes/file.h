#ifndef FILE_DATA_H_
#define FILE_DATA_H_

#include <gtk/gtk.h>

class EditArea;

namespace datatypes{

typedef struct FileData{
    GFile *file;
    GFileInfo *fileInfo;
    EditArea *editArea;
    char *fileName;
    char *fileAbsoPath;
    GIcon *icon;
}FileData;

}
#endif
