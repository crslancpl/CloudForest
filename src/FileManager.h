#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#include "DataTypes.h"
#pragma once

#include <gtk/gtk.h>

void InitFileManager(MainWindow *parent);
void OpenFileChooser(bool FileOrDir);
static void FileSelected(GObject *source, GAsyncResult *result, void *data);
static void FolderSelected(GObject *source, GAsyncResult *result, void *data);
static void OpenFile(GFile *File);
static void ReadFolder(GFile *Folder, bool IsRoot);

#endif
