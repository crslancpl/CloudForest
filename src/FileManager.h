#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#pragma once

#include "Classes.h"
#include <memory>


#include <gtk/gtk.h>

void InitFileManager(MainWindow *parent);
void OpenFileChooser(bool FileOrDir);
void FileSelected(GObject *source, GAsyncResult *result, void *data);// Open file and create a EditArea
void FolderSelected(GObject *source, GAsyncResult *result, void *data);// Show folder in FilePanel
void OpenFile(GFile &file, FPFileButton* f);// Open file and create a EditArea
void ReadAsRootFolder(GFile &folder);
void ReadFolder(GFile &folder,FPFolderButton &folderbutton);// ReadFolder() is called when user expand the folder in FilePanel
void CreateFile(EditArea& ea);// Show a location choosing window and create a new file with the content inside EditArea
void FileCreated(GObject* source, GAsyncResult *result, void* data);

#endif
