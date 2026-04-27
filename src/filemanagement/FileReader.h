#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <gtk/gtk.h>

//forward definition

typedef struct FileData FileData;

namespace filemanagement{

void EnumerateFolderChild(GFile* folder, void (*callback)(GFile*, GFileInfo*));
FileData* LoadFileData(GFile* file);
void ReadFileText(GFile* file, char** output);

}// namespace filemanagement

#endif
