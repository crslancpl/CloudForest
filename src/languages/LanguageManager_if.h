#ifndef LANGUAGEMANAGER_IF_H_
#define LANGUAGEMANAGER_IF_H_

#include "src/gui/editarea/EditArea.h"
#include "toolset/event/Event.h"
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

enum Signal{
    LANG_MANAGER_NEW_LANG,
};

void Listen(Signal signal, EventCallback callback);
void StopListen(Signal signal, EventCallback callback);

}

#endif
