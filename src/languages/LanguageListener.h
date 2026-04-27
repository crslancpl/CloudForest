#ifndef LANGUAGELISTENER_H_
#define LANGUAGELISTENER_H_

class EditArea;

typedef struct Language Language;


void LanguageListenerStart();

void LanguageNewEditArea(EditArea* ea, Language* lang);
void ListenNewEditAreaForLanguage(const char* langname, void (*callback)(const char*, EditArea*));



#endif
