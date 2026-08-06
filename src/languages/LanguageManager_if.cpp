#include "LanguageManager_if.h"

#include "datatypes/common.h"
#include "toolset/event/Event.h"
#include "toolset/tools/Tool.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

typedef void (*NewLanguageCallback)(const Language*);

static const Language* unknown_lang = nullptr;

static std::vector<std::unique_ptr<Language>> language_list = {};// has ownership

static std::unordered_map<std::string, const Language*> file_extension_to_language_map = {};

static std::unordered_map<std::string, const Language*> lang_name_to_language_map = {};



namespace langmanager{

static std::unordered_map<Signal, SimpleEvent> event_map = {
    {LANG_MANAGER_NEW_LANG, SimpleEvent()}
};

void Init(){
    std::unique_ptr<Language> unknown = std::make_unique<Language>();
    unknown->id = "unknown";
    unknown->name = "Unknown";
    unknown->fileExtensions = {};
    unknown->syntaxTemplateFile = "unknown.txt";

    lang_name_to_language_map.emplace("Unknown", unknown.get());
    language_list.emplace_back(std::move(unknown));
}

void ClearLanguageList(){
    language_list.clear();
}

void AddToLanguageList(std::unique_ptr<Language> lang){
    Language* langrawptr = lang.get();
    language_list.emplace_back(std::move(lang));

    lang_name_to_language_map.emplace(langrawptr->name, langrawptr);

    for (const std::string& fileext: langrawptr->fileExtensions) {
        file_extension_to_language_map.emplace(fileext, langrawptr);
    }

    const SimpleEvent &event = event_map.at(LANG_MANAGER_NEW_LANG);
    for(EventCallback callback : event.GetCallbackSet()){
        ((NewLanguageCallback)callback)(langrawptr);
    }
}

const Language* FindByName(const char* langname){
    auto result = lang_name_to_language_map.find(langname);
    return result != lang_name_to_language_map.end() ? result->second : unknown_lang;
}

const Language* FindByFileExtension(const char* filename){
    auto file_ext_pair = tools::TrimText(filename, ".");//"/path/file.txt" -> "/path/file" and "txt"
    if(file_ext_pair.size() != 2){
        return unknown_lang;
    }

    auto result = file_extension_to_language_map.find(file_ext_pair[1]);
    return result != file_extension_to_language_map.end() ? result->second : unknown_lang;
}

const std::unordered_map<std::string, const Language*> &GetLanguageMap(){
    return lang_name_to_language_map;
}

void Listen(Signal signal, void (*callback)()){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void StopListen(Signal signal, void (*callback)()){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}

}// namespace langmanager
