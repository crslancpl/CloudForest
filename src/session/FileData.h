#ifndef FILEDATA_H_
#define FILEDATA_H_

#include "datatypes/file.h"
#include "src/filemanagement/FileTree.h"

#include <unordered_set>

namespace session {

Workspace* NewWorkspace(FileData* rootfolderdata);
const std::unordered_set<Workspace*> &GetWorkspaceList();
Workspace* FindWorkspaceByFileData(FileData* filedata);
Workspace* FindWorkspaceByPath(const char* path);

void AddSingleFile(FileData* filedata);
FileData* FindSingleFileByPath(const char* path);

} // namespace session

#endif
