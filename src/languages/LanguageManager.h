#ifndef LANGUAGEMANAGER_H_
#define LANGUAGEMANAGER_H_


#include "Language.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"
#include "toolset/lspbackend/LspClient.h"

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

    LspClient& StartLsp(Language* lang, const std::string& command);

    Language* FindByName(const std::string& name) const;
    Language* FindByFileExtension(const std::string& name) const;

    const std::vector<std::unique_ptr<Language>>& GetLanguages() const;

    Event<Language*> ENewLanguage;

private:
    AppContext& m_context;

    Language* m_unknownLang;

    std::vector<std::unique_ptr<Language>> m_languages;
    std::unordered_map<std::string, Language*> m_langNameToLangMap;
    std::unordered_map<std::string, Language*> m_fileExtensionToLangMap;

    std::unordered_map<const Language*, std::unique_ptr<LspClient>> m_lspClients;
};

#endif
