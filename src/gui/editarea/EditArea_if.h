#ifndef EDITAREA_IF_H_
#define EDITAREA_IF_H_

#include "datatypes/common.h"

#include <gtk/gtk.h>
#include <string>

//forward declaration
class EditArea;
typedef struct FileData FileData ;


namespace editarea {
void OpenFile(FileData* file);//pass nullptr to create empty file
void CreateEmptyFile();
void OpenFileWithAbsoPath(const std::string &absoPath);
void CloseFile(FileData* file);
void SaveFile(FileData* file);
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

void ListenEvent(Event event, EventCallback callback);
void StopListenEvent(Event event, EventCallback callback);

}// namespace editarea

#endif
