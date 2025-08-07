#pragma once
#include <gtk/gtk.h>
#include <memory>
#include "Classes.h"
void ToggleFolder(GtkButton* self,Folder *F);
void FileButtonClick(GtkButton *self,File &Parent);
