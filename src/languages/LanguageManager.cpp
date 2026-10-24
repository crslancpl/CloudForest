#include "LanguageManager.h"

#include "Language.h"
#include "datatypes/common.h"
#include "src/AppContext.h"
#include "toolset/event/Event.h"
#include "toolset/lspbackend/LspClient.h"
#include "toolset/tools/Tool.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

typedef void (*NewLanguageCallback)(Language*);

LanguageManager::LanguageManager(AppContext& context) :
    m_context(context)
{
    std::unique_ptr<LanguageInfo> unknown = std::make_unique<LanguageInfo>();
    unknown->id = "unknown";
    unknown->name = "Unknown";
    unknown->fileExtensions = {};
    unknown->syntaxTemplateFile = "unknown.txt";

    m_unknownLang = &this->CreateLanguage(std::move(unknown));
}

LanguageManager::~LanguageManager(){}

Language& LanguageManager::CreateLanguage(std::unique_ptr<LanguageInfo> langinfo){
    std::unique_ptr<Language> newlang = std::make_unique<Language>(std::move(langinfo));
    Language* raw = newlang.get();
    const LanguageInfo* info = raw->GetInfo();

    // storing new language to containers
    m_languages.emplace_back(std::move(newlang));
    m_langNameToLangMap.emplace(info->name, raw);
    for (const std::string& fileext: info->fileExtensions) {
        m_fileExtensionToLangMap.emplace(fileext, raw);
    }

    ENewLanguage.Invoke(raw);

    return *raw;
}

void LanguageManager::ClearLanguages(){
    m_languages.clear();
}

LspClient& LanguageManager::StartLsp(Language* lang, const std::string& command){
    std::unique_ptr<LspClient> cl = std::make_unique<LspClient>(command, lang->GetInfo()->id);
    LspClient& ref = *cl;
    m_lspClients.emplace(lang, std::move(cl));
    return ref;
}


Language* LanguageManager::FindByName(const std::string& langname) const{
    auto result = m_langNameToLangMap.find(langname);
    return result != m_langNameToLangMap.end() ? result->second : m_unknownLang;
}

Language* LanguageManager::FindByFileExtension(const std::string& filename) const{
    auto file_ext_pair = tools::TrimText(filename, ".");//"/path/file.txt" -> "/path/file" and "txt"
    if(file_ext_pair.size() != 2){
        return m_unknownLang;
    }

    auto result = m_fileExtensionToLangMap.find(file_ext_pair[1]);
    return result != m_fileExtensionToLangMap.end() ? result->second : m_unknownLang;
}

const std::vector<std::unique_ptr<Language>> &LanguageManager::GetLanguages() const{
    return m_languages;
}
