#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_
#pragma once

#include <gtk/gtk.h>

void InitFileManager(GtkWindow *parent);
void OpenFileChooser(bool FileOrDir);
static void FileOpened(GObject *source, GAsyncResult *result, void *data);

#endif
