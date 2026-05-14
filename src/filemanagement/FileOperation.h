#ifndef FILEOPERATION_H_
#define FILEOPERATION_H_

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <unordered_set>

// forward declare
typedef struct FileData FileData;
typedef struct Workspace Workspace;

void FileOperationInit();

namespace filemanagement{

FileData* CreateFile(char* name, FileData* parentdata);//create file in file panel
FileData* CreateVirtualFile();
void SaveFile(FileData* filedata, char* content, void (*savedcallback)(FileData*));

void ChooseFile();
void ChooseFolder();
void NewWorkspace(FileData* rootfolderdata);
const std::unordered_set<Workspace*> &GetWorkspaceList();
Workspace* FindWorkspace(FileData* filedata);
Workspace* FindWorkspaceFromPath(const char* path);
void OpenFile(FileData* filedata);
void CloseFile(FileData *filedata);

}// namespace filemangement

#endif
