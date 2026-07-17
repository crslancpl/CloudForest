#ifndef FILEDATA_H_
#define FILEDATA_H_

#include "datatypes/file.h"
#include "src/filemanagement/FileTree.h"

#include <memory>
#include <unordered_set>

namespace session {

Workspace* NewWorkspace(std::unique_ptr<FileData> rootfolderdata);
const std::unordered_set<std::unique_ptr<Workspace>> &GetWorkspaceList();
Workspace* FindWorkspaceByFileData(FileData* filedata);
Workspace* FindWorkspaceByPath(const char* path);

void AddSingleFile(std::unique_ptr<FileData> filedata);
void RemoveSingleFile(FileData* filedata);
FileData* FindSingleFileByPath(const char* path);

} // namespace session

#endif
