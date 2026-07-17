#ifndef FILEOPERATION_H_
#define FILEOPERATION_H_

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>

// forward declare
typedef struct FileData FileData;
typedef struct App App;

void FileOperationInit(App& app);

namespace filemanager{

std::unique_ptr<FileData> CreateFile(const char* name, FileData* parentdata);//create file in file panel
std::unique_ptr<FileData> CreateNewFile();
std::unique_ptr<FileData> CreateVirtualFile();
std::unique_ptr<FileData> CreateVirtualFolder();

void SaveFile(FileData* filedata, const char* content, void (*savedcallback)(FileData*));

void ChooseFile();
void ChooseFolder();

}// namespace filemanager

#endif
