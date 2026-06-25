#ifndef EDITAREADATA_H_
#define EDITAREADATA_H_

#include "datatypes/common.h" // for Language
#include "editarea/EditArea.h" // for EditArea
#include "src/filemanagement/FileTree.h"
#include "src/languages/LanguageGroup.h"

#include <unordered_set>

namespace session{

void InitEditAreaData();

Workspace* FindWorkspaceByPath();

const std::unordered_set<EditArea*> &GetAllEditAreas();
const std::unordered_set<EditArea*> &GetEditAreasByLanguage(const Language* lang);
EditArea *FindEditAreaByFileData(const FileData* file);
EditArea *FindEditAreaByPath(const char* absopath);

void EditNewFile();
void EditFile(FileData* file);
void CloseFile(FileData* file);

EditArea* GetFocusedEditArea();
void SetFocusedEditArea(EditArea* editarea);
LanguageGroup* FindLanguageGroup(const Language* lang);

}// namespace session

#endif
