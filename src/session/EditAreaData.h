#ifndef EDITAREADATA_H_
#define EDITAREADATA_H_

#include "datatypes/common.h" // for Language
#include "editarea/EditArea.h" // for EditArea
#include "src/filemanagement/FileTree.h"

#include <unordered_set>

namespace session{

class EditAreaSet{
    /*
     * automatically erase the EditArea from the set
     * when the EditArea is closed.
     */
public:
    const std::unordered_set<EditArea*> &GetEditAreaSet() const;
    void Add(EditArea* ea);
    void ManualRemove(EditArea* ea);

private:
    std::unordered_set<EditArea*> m_editareaSet;
};

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

}// namespace session

#endif
