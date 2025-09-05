#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#include "Core.h"
#pragma once

#include "Classes.h"
#include <memory>
#include <string>

#include <gtk/gtk.h>


namespace filemanag{
void Init();
void Process(FileAction* action);
void OpenFileChooser(bool FileOrDir);
void LoadText(GFile* file,char* &textoutput);//load the content to textoutput
void EnumerateFolderChild(GFile* folder);
}


void OpenFile(GFile *file, FPFileButton* f);// Open file and create a EditArea
void ReadAsRootFolder(GFile &folder);
void ReadFolder(GFile *folder,FPFolderButton &folderbutton);// ReadFolder() is called when user expand the folder in FilePanel
void CreateFile(EditArea& ea);// Show a location choosing window and create a new file with the content inside EditArea
void FileCreated(GObject* source, GAsyncResult *result, void* data);

#endif
