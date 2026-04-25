#include "LanguageManager_if.h"

#include "datatypes/language.h"
#include "toolset/tools/text_tool.h"
#include "LanguageListener.h"

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

static std::unordered_map<std::string, datatypes::Language*> extension_map = {};

void langmanager::Init(){
    LanguageListenerStart();
}

void langmanager::Clear(){
    language_list.clear();
}

void langmanager::NewLanguage(const std::string& langname, const std::string& id, const std::string& syntaxfile, const std::string& fileextensions){
    datatypes::Language *lang = new datatypes::Language();
    lang->name = langname;
    lang->id = id;
    lang->syntaxTemplateFile = syntaxfile;

    auto extlist = tools::TrimText(fileextensions, "[],' ");
    for(auto ext : extlist){
        extension_map.emplace(ext,lang);
    }
    language_list.emplace(langname, lang);

    for(auto callback : new_lang_callback_list){
        callback(lang);
    }
}

datatypes::Language* langmanager::FindLanguage(const char* langname){
    auto result = language_list.find(langname);
    return result != language_list.end() ? result->second : &unknow_lang;
}

datatypes::Language* langmanager::FindFileLanguage(const char* filename){
    auto file_ext_pair = tools::TrimText(filename, ".");//"/path/file.txt" -> "/path/file" and "txt"

    if(file_ext_pair.size() != 2){
        return &unknow_lang;
    }

    auto result = extension_map.find(file_ext_pair[1]);
    return result != language_list.end() ? result->second : &unknow_lang;
}

void langmanager::AddNewLangCallback(void (*callback)(datatypes::Language*)){
    new_lang_callback_list.emplace(callback);
}
void langmanager::RemoveNewLangCallback(void (*callback)(datatypes::Language*)){
    new_lang_callback_list.erase(callback);
}
