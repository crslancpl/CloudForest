#ifndef LANGUAGELISTENER_H_
#define LANGUAGELISTENER_H_

class EditArea;

typedef struct Language Language;

namespace langmanager{

void StartListener();

void AddEditArea(EditArea* ea, Language* lang);
void ListenNewEditArea(const char* langname, void (*callback)(const char*, EditArea*));

}// namespace langmanager

#endif
