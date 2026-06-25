#include "LanguageManager_if.h"

#include "datatypes/common.h"
#include "toolset/event/Event.h"
#include "toolset/tools/Tool.h"
#include "LanguageListener.h"

#include <unordered_map>
#include <string>

typedef void (*NewLanguageCallback)(Language*);

static std::unordered_map<std::string, Language*> file_extension_to_language_map = {};

Language unknow_lang = {
    .name = "Unknown",
    .id = "unknown",
    .fileExtensions = {},
    .syntaxTemplateFile = "unknown.txt"
};

static std::unordered_map<std::string, Language*> language_list = {
    {"Unknown", &unknow_lang}
};



namespace langmanager{

std::unordered_map<Signal, SimpleEvent> event_map = {
    {LANG_MANAGER_NEW_LANG, SimpleEvent()}
};

void Init(){
    //StartListener();
}

void ClearLanguageList(){
    language_list.clear();
}

void AddToLanguageList(Language* lang){
    language_list.emplace(lang->name, lang);
    for (const std::string& fileext: lang->fileExtensions) {
        file_extension_to_language_map.emplace(fileext, lang);
    }

    const SimpleEvent &event = event_map.at(LANG_MANAGER_NEW_LANG);
    for(EventCallback callback : event.GetCallbackSet()){
        ((NewLanguageCallback)callback)(lang);
    }
}

Language* FindByName(const char* langname){
    auto result = language_list.find(langname);
    return result != language_list.end() ? result->second : &unknow_lang;
}

Language* FindByFileExtension(const char* filename){
    auto file_ext_pair = tools::TrimText(filename, ".");//"/path/file.txt" -> "/path/file" and "txt"
    if(file_ext_pair.size() != 2){
        return &unknow_lang;
    }

    auto result = file_extension_to_language_map.find(file_ext_pair[1]);
    return result != language_list.end() ? result->second : &unknow_lang;
}

const std::unordered_map<std::string, Language*>& GetLanguageList(){
    return language_list;
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
