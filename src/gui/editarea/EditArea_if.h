#ifndef EDITAREA_IF_H_
#define EDITAREA_IF_H_

#include "datatypes/common.h"

#include <gtk/gtk.h>

//forward declaration
class EditArea;
typedef struct FileData FileData ;


namespace editarea {
void EditFile(FileData* file);//pass nullptr to create empty file
void EditNewFile();

void CloseFile(FileData* file);

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
