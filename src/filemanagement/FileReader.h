#ifndef FILEREADER_H_
#define FILEREADER_H_

#include "datatypes/file.h"

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>

class FolderBranch;

namespace filemanager{

void ExpandFolderBranch(FolderBranch* branch);

std::unique_ptr<FileData> LoadFileData(GFile* filedata, GFileInfo* info, bool issinglefile);
void ReadFileText(FileData* filedata, char** output);

}// namespace filemanager

#endif
