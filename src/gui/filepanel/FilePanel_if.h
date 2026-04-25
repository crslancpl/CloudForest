#ifndef FILEPANEL_IF_H_
#define FILEPANEL_IF_H_

#include <gtk/gtk.h>

class FPFolderButton;

namespace filepanel{
void LoadAsRoot(GFile *file, GFileInfo *fileinfo);
void Construct();
FPFolderButton* GetFolderButton(const GFile *folder);
}

#endif
