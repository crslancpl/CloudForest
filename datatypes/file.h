#ifndef FILE_DATA_H_
#define FILE_DATA_H_

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <string>

class Branch;
class EditArea;
class FilePanelButton;


typedef struct FileData{
    bool isVirtual;
    bool isSingleFile;// is open as a single file or open from a folder in file panel
    unsigned long int sortingCode1;
    unsigned long int sortingCode2;
    unsigned long int sortingCode3;
    GFileType type;
    GFile *file;
    GFileInfo *fileInfo;
    FilePanelButton *filePanelButton;
    Branch *branch;
    EditArea *editArea;
    std::string fileName;
    std::string absoPath;
    std::string extension;
    GIcon *icon;
}FileData;

#endif
