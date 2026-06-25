#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <gio/gio.h>
#include <gtk/gtk.h>

class FolderBranch;

typedef struct FileData FileData;

namespace filemanager{

void ExpandFolderBranch(FolderBranch* branch);

FileData* LoadFileData(GFile* filedata, GFileInfo* info, bool issinglefile);
void ReadFileText(FileData* filedata, char** output);

}// namespace filemanager

#endif
