#ifndef FILE_DATA_H_
#define FILE_DATA_H_

#include <gio/gio.h>
#include <gtk/gtk.h>

class EditArea;

typedef struct FileData{
    bool isVirtual;
    unsigned long int sortingCode1;
    unsigned long int sortingCode2;
    unsigned long int sortingCode3;
    GFileType type;
    GFile *file;
    GFileInfo *fileInfo;
    EditArea *editArea;
    char *fileName;
    char *absoPath;
    char* *extension;
    GIcon *icon;
}FileData;

#endif
