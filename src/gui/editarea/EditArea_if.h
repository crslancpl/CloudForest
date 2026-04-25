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

enum Event{
    EDITAREA_CREATED,
    EDITAREA_FOCUSED_CHANGED,
    EDITAREA_LANG_CHANGED
};

void ListenEvent(Event event, void (*callback)());
void StopListenEvent(Event event, void (*callback)());

}// namespace editarea

#endif
