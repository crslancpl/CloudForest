#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <gio/gio.h>
#include <gtk/gtk.h>

//forward declare

typedef struct FileData FileData;
typedef struct FileBranch FileBranch;

namespace filemanagement{

FileBranch* CreateFileTree(FileData* rootfolderdata);
void ExpandFileBranch(FileBranch* branch);

FileData* LoadFileData(GFile* filedata, GFileInfo* info);
void ReadFileText(FileData* filedata, char** output);

}// namespace filemanagement

#endif
