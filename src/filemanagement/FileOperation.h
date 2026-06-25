#ifndef FILEOPERATION_H_
#define FILEOPERATION_H_

#include <gio/gio.h>
#include <gtk/gtk.h>

// forward declare
typedef struct FileData FileData;

void FileOperationInit();

namespace filemanager{

FileData* CreateFile(char* name, FileData* parentdata);//create file in file panel
FileData* CreateNewFile();
FileData* CreateVirtualFile();
FileData* CreateVirtualFolder();

void SaveFile(FileData* filedata, char* content, void (*savedcallback)(FileData*));

void ChooseFile();
void ChooseFolder();

}// namespace filemanager

#endif
