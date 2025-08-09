#ifndef CORE_H_
#define CORE_H_
#pragma once

#include "Classes.h"

MainWindow &GetAppWindow();



shared_ptr<FPFileButton> NewFileButton();// The newly created file button will be pushed to AllFileButtons
shared_ptr<FPFolderButton> NewFolderButton();// The newly created folder button will be pushed to AllFolderButtons

shared_ptr<EditArea>& NewEditArea(GFile* filetoedit, FPFileButton* filebutton);
void RemoveEditArea(EditArea* EditAreaPtr);
void AddEditArea(shared_ptr<EditArea>& EditAreaPtr);
shared_ptr<EditArea> GetEditAreaFromFileAbsoPath(const string &AbsPath);

void RemoveFolder(FPFolderButton& folder);// Not nullable


#endif
