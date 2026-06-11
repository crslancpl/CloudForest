#ifndef LANGUAGEMANAGER_IF_H_
#define LANGUAGEMANAGER_IF_H_

#include "src/gui/editarea/EditArea.h"
#include <string>
#include <unordered_map>

typedef struct Language Language;


namespace langmanager{
void Init();

void Clear();

//called by python srcipt `pythonscripts/language/lang_loader.py"
void NewLanguage(Language* lang);

void UpdateEditAreaLanguage(const EditArea* ea, Language* lang);

Language* FindLanguage(const char* langname);
Language* FindFileLanguage(const char* filename);

const std::unordered_set<const EditArea*>& GetEditAreasFromLanguage(const Language* lang);
const std::unordered_map<std::string, Language*>& GetLanguageList();

enum Event{
    LANG_MANAGER_NEW_LANG,
};

void ListenEvent(Event event, void (*callback)());
void StopListenEvent(Event event, void (*callback)());

}

#endif
