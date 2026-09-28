#include "LanguageManager.h"

#include "datatypes/common.h"
#include "src/AppContext.h"
#include "toolset/event/Event.h"
#include "toolset/tools/Tool.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

typedef void (*NewLanguageCallback)(const Language*);

static const Language* unknown_lang = nullptr;

static std::unordered_map<std::string, const Language*> lang_name_to_language_map = {};

LanguageManager::LanguageManager(AppContext& context) :
    m_context(context)
    {
    std::unique_ptr<Language> unknown = std::make_unique<Language>();
    unknown->id = "unknown";
    unknown->name = "Unknown";
    unknown->fileExtensions = {};
    unknown->syntaxTemplateFile = "unknown.txt";

    lang_name_to_language_map.emplace("Unknown", unknown.get());
    m_languages.emplace_back(std::move(unknown));
}

LanguageManager::~LanguageManager(){}

void LanguageManager::ClearLanguageList(){
    m_languages.clear();
}

void LanguageManager::AddToLanguageList(std::unique_ptr<Language> lang){
    Language* langrawptr = lang.get();
    m_languages.emplace_back(std::move(lang));

    lang_name_to_language_map.emplace(langrawptr->name, langrawptr);

    for (const std::string& fileext: langrawptr->fileExtensions) {
        m_fileExtensionToLangMap.emplace(fileext, langrawptr);
    }

    const SimpleEvent &event = event_map.at(LANG_MANAGER_NEW_LANG);
    for(EventCallback callback : event.GetCallbackSet()){
        ((NewLanguageCallback)callback)(langrawptr);
    }
}

const Language* LanguageManager::FindByName(const char* langname){
    auto result = lang_name_to_language_map.find(langname);
    return result != lang_name_to_language_map.end() ? result->second : unknown_lang;
}

const Language* LanguageManager::FindByFileExtension(const char* filename){
    auto file_ext_pair = tools::TrimText(filename, ".");//"/path/file.txt" -> "/path/file" and "txt"
    if(file_ext_pair.size() != 2){
        return unknown_lang;
    }

    auto result = m_fileExtensionToLangMap.find(file_ext_pair[1]);
    return result != m_fileExtensionToLangMap.end() ? result->second : unknown_lang;
}

const std::unordered_map<std::string, const Language*> &LanguageManager::GetLanguageMap(){
    return lang_name_to_language_map;
}

void LanguageManager::Listen(Signal signal, void (*callback)()){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void LanguageManager::StopListen(Signal signal, void (*callback)()){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}
