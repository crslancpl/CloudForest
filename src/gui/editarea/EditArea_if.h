#ifndef EDITAREA_IF_H_
#define EDITAREA_IF_H_

#include "toolset/event/Event.h"

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

enum Signal{
    EDITAREA_CREATED,
    EDITAREA_FOCUSED_CHANGED,
    EDITAREA_LANG_CHANGED
};

void Listen(Signal signal, EventCallback callback);
void StopListen(Signal signal, EventCallback callback);

}// namespace editarea

#endif
