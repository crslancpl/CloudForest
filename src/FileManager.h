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
void Process(Request* request);
void OpenFileChooser(bool FileOrDir);
void Save(GFile *file,std::string* content);
void LoadText(GFile* file,char* &textoutput);//load the content to textoutput
void EnumerateFolderChild(GFile* folder);
}


#endif
