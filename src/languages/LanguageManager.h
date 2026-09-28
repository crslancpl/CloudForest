#ifndef LANGUAGEMANAGER_H_
#define LANGUAGEMANAGER_H_

#include "toolset/event/Event.h"

#include <unordered_map>
#include <string>
#include <memory>

typedef struct Language Language;
typedef struct AppContext AppContext;

class LanguageManager : public EventReceiver{
public:
    LanguageManager(AppContext& context);
    ~LanguageManager();

    //called by python srcipt `pythonscripts/language/lang_loader.py"
    void AddToLanguageList(std::unique_ptr<Language> lang);
    void ClearLanguageList();

    const Language* FindByName(const char* langname);
    const Language* FindByFileExtension(const char* filename);

    const std::unordered_map<std::string, const Language*>& GetLanguageMap();

    enum Signal{
        LANG_MANAGER_NEW_LANG,
    };

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);
private:
    AppContext& m_context;

    std::vector<std::unique_ptr<Language>> m_languages = {};
    std::unordered_map<std::string, const Language*> m_fileExtensionToLangMap;

    std::unordered_map<Signal, SimpleEvent> event_map = {
        {LANG_MANAGER_NEW_LANG, SimpleEvent()}
    };
};

#endif
