#ifndef LANGUAGEMANAGER_IF_H_
#define LANGUAGEMANAGER_IF_H_


#include "toolset/event/Event.h"

#include <string>
#include <unordered_map>

typedef struct Language Language;


namespace langmanager{
void Init();

//called by python srcipt `pythonscripts/language/lang_loader.py"
void AddToLanguageList(Language* lang);
void ClearLanguageList();

Language* FindByName(const char* langname);
Language* FindByFileExtension(const char* filename);

const std::unordered_map<std::string, Language*>& GetLanguageList();

enum Signal{
    LANG_MANAGER_NEW_LANG,
};

void Listen(Signal signal, EventCallback callback);
void StopListen(Signal signal, EventCallback callback);

}// namespace langmanager

#endif
