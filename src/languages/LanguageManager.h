#ifndef LANGUAGEMANAGER_H_
#define LANGUAGEMANAGER_H_


#include "Language.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

typedef struct AppContext AppContext;

class LanguageManager : public EventReceiver {
public:
    LanguageManager(AppContext& context);
    ~LanguageManager();

    // Called by python srcipt `pythonscripts/language/lang_loader.py"
    // AddToLanguages
    Language& CreateLanguage(std::unique_ptr<LanguageInfo> langinfo);
    void ClearLanguages();

    Language* FindByName(const std::string& name) const;
    Language* FindByFileExtension(const std::string& name) const;

    const std::vector<std::unique_ptr<Language>>& GetLanguages() const;

    enum Signal{
        LANG_MANAGER_NEW_LANG,
    };

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);

private:
    void EditAreaLangChanged(EditArea* ea);

    AppContext& m_context;

    Language* m_unknownLang;

    std::vector<std::unique_ptr<Language>> m_languages;
    std::unordered_map<std::string, Language*> m_langNameToLangMap;
    std::unordered_map<std::string, Language*> m_fileExtensionToLangMap;

    std::unordered_map<Signal, SimpleEvent> event_map = {
        {LANG_MANAGER_NEW_LANG, SimpleEvent()}
    };
};

#endif
