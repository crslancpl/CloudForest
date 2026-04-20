#ifndef LANGUAGELISTENER_H_
#define LANGUAGELISTENER_H_

class EditArea;

namespace datatypes{
typedef struct Language Language;
}

void LanguageNewEditArea(EditArea* ea, datatypes::Language* lang);

void ListenNewEditAreaForLanguage(const char* langname, void (*callback)(EditArea*));

#endif
