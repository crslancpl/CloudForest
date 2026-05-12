#ifndef FILE_DATA_H_
#define FILE_DATA_H_

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <unordered_set>

class EditArea;

typedef struct FileData{
    bool isVirtual;
    GFileType type;
    GFile *file;
    GFileInfo *fileInfo;
    EditArea *editArea;
    char *fileName;
    char *absoPath;
    char* *extension;
    GIcon *icon;
}FileData;

typedef struct FileBranch{
    char* name;
    FileData* fileData;
    FileBranch* parentBranch;
    std::unordered_set<FileBranch*> childBranch;//only for directories
} FileBranch;

typedef struct Workspace{
    char* name;// can be named by user
    FileData* rootFolderData;
    FileBranch* wsBranch;
}Workspace;

#endif
