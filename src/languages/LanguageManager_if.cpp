#include "LanguageManager_if.h"

#include "datatypes/language.h"

#include <unordered_map>
#include <string>

static std::unordered_set<void(*)(datatypes::Language*)> new_lang_callback_list = {};

datatypes::Language unknow_lang = {
    .name = "Unknown",
    .id = "unknown",
    .fileExtensions = {},
    .syntaxTemplateFile = "unknown.txt"
};

static std::unordered_map<std::string, datatypes::Language*> language_list = {
    {"Unknown", &unknow_lang}
};

void langmanager::Clear(){
    language_list.clear();
}

void langmanager::NewLanguage(const std::string& langname, const std::string& id, const std::string& syntaxfile){
    datatypes::Language *lang = new datatypes::Language();
    lang->name = langname;
    lang->id = id;
    lang->syntaxTemplateFile = syntaxfile;

    language_list.emplace(langname, lang);
    for(auto callback : new_lang_callback_list){
        callback(lang);
    }
}

datatypes::Language* langmanager::FindLanguage(const char* langname){
    auto result = language_list.find(langname);
    return result != language_list.end() ? result->second : &unknow_lang;
}

void langmanager::AddNewLangCallback(void (*callback)(datatypes::Language*)){
    new_lang_callback_list.emplace(callback);
}
void langmanager::RemoveNewLangCallback(void (*callback)(datatypes::Language*)){
    new_lang_callback_list.erase(callback);
}
