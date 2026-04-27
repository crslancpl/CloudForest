#ifndef FILEOPERATION_H_
#define FILEOPERATION_H_

#include <gtk/gtk.h>

void FileOperationInit();

namespace filemanagement{

GFile* CreateFile(char* name, GFile* parentfolder);//create file in file panel
void SaveFile(GFile* file, char* content, void (*savedcallback)(GFile*));

void ChooseFile();
void ChooseFolder();

}// namespace filemangement

#endif
