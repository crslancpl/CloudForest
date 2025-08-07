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
void OpenFile(GFile &File);
void ReadAsRootFoler(GFile &folder);
void ReadFolder(GFile &folder,Folder &F);// Folder F is not nullable


#endif
