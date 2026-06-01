#ifndef LANGUAGEMANAGER_IF_H_
#define LANGUAGEMANAGER_IF_H_

#include <string>
#include <unordered_map>

typedef struct Language Language;


namespace langmanager{
void Init();

void Clear();
void NewLanguage(const std::string& langname, const std::string& id, const std::string& syntaxfile, const std::string& fileextensions);//called by python srcipt `pythonscripts/language/lang_loader.py"
Language* FindLanguage(const char* langname);
Language* FindFileLanguage(const char* filename);
const std::unordered_map<std::string, Language*>& GetLanguageList();

enum Event{
    LANG_MANAGER_NEW_LANG,
};

void ListenEvent(Event event, void (*callback)());
void StopListenEvent(Event event, void (*callback)());

}

#endif
