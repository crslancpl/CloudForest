#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#pragma once

#include "Classes.h"
#include <memory>


#include <gtk/gtk.h>

void InitFileManager(MainWindow *parent);
void OpenFileChooser(bool FileOrDir);
void FileSelected(GObject *source, GAsyncResult *result, void *data);
void FolderSelected(GObject *source, GAsyncResult *result, void *data);
void OpenFile(GFile &file, FPFileButton* f);
void ReadAsRootFoler(GFile &folder);
void ReadFolder(GFile &folder,FPFolderButton &F);// Folder F is not nullable
void CreateFile(EditArea& ea);
void FileCreated(GObject* source, GAsyncResult *result, void* data);

#endif
