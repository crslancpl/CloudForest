#ifndef EDITAREA_IF_H_
#define EDITAREA_IF_H_

#include <gtk/gtk.h>
#include <string>

class EditArea;


namespace editarea {
    void OpenFile(GFile* file);//pass nullptr to create empty file
    void CreateEmptyFile();
    void OpenFileWithAbsoPath(const std::string &absoPath);
    void CloseFile(GFile* file);
    void SaveFile(GFile* file);
    EditArea* GetFocusedEditArea();
    void SetFocusedEditArea(EditArea* editarea);

    void InsertToEditAreaList(EditArea* ea);
    void RemoveFromEditAreaList(EditArea* ea);
    EditArea *FindEditArea(const char* absopath);

    void AddNewEditAreaCallback(void (*callback)(EditArea*));
    void RemoveNewEditAreaCallback(void (*callback)(EditArea*));
    void AddFocusChangedCallback(void (*callback)(EditArea*));
    void RemoveFocusChangedCallback(void (*callback)(EditArea*));
    void AddLangChangedCallback(void (*callback)(EditArea*,const char*));// the second parameter of the callback is the old language
    void RemoveLangChangedCallback(void (*callback)(EditArea*, const char*));
}

#endif
