#include "LanguageManager_if.h"

#include "datatypes/common.h"
#include "pythonbackend/language_mod_Py.h"
#include "src/gui/editarea/EditArea.h"
#include "toolset/tools/Tool.h"
#include "LanguageListener.h"

#include <cstdio>
#include <unordered_map>
#include <string>
#include <unordered_set>

static std::unordered_map<const Language*, std::unordered_set<const EditArea*>> lang_to_editarea_map = {};
static std::unordered_map<const EditArea*, const Language*> editarea_to_lang_map = {};

static std::unordered_set<void(*)(Language*)> new_lang_callback_list = {};

Language unknow_lang = {
    .name = "Unknown",
    .id = "unknown",
    .fileExtensions = {},
    .syntaxTemplateFile = "unknown.txt"
};

static std::unordered_map<std::string, Language*> language_list = {
    {"Unknown", &unknow_lang}
};

static std::unordered_map<std::string, Language*> file_extension_to_language_map = {};

namespace langmanager{

void Init(){
    LanguageListenerStart();
}

void Clear(){
    language_list.clear();
}

void NewLanguage(Language* lang){
    language_list.emplace(lang->name, lang);
    for (const std::string& fileext: lang->fileExtensions) {
        file_extension_to_language_map.emplace(fileext, lang);
    }

    for(auto callback : new_lang_callback_list){
        callback(lang);
    }
}

void UpdateEditAreaLanguage(const EditArea* ea, Language* lang){
    auto itr = editarea_to_lang_map.find(ea);

    if(itr != editarea_to_lang_map.end()){
        const Language* oldlang = itr->second;
        lang_to_editarea_map[oldlang].erase(ea);
    }

    if(!lang->used){
        language_module_invoke_language_used(lang->name);
        lang->used = true;
    }

    editarea_to_lang_map.emplace(ea, lang);
    lang_to_editarea_map[lang].emplace(ea);
}

Language* FindLanguage(const char* langname){
    auto result = language_list.find(langname);
    return result != language_list.end() ? result->second : &unknow_lang;
}

Language* FindFileLanguage(const char* filename){
    auto file_ext_pair = tools::TrimText(filename, ".");//"/path/file.txt" -> "/path/file" and "txt"
    if(file_ext_pair.size() != 2){
        return &unknow_lang;
    }

    auto result = file_extension_to_language_map.find(file_ext_pair[1]);
    return result != language_list.end() ? result->second : &unknow_lang;
}

const std::unordered_set<const EditArea*>& GetEditAreasFromLanguage(const Language* lang){
    return lang_to_editarea_map[lang];
}

const std::unordered_map<std::string, Language*>& GetLanguageList(){
    return language_list;
}

void ListenEvent(Event event, void (*callback)()){
    switch (event) {
    case LANG_MANAGER_NEW_LANG:
        new_lang_callback_list.emplace((void(*)(Language*))callback);
        break;
    default:
        break;
    }
}

void StopListenEvent(Event event, void (*callback)()){
    switch (event) {
    case LANG_MANAGER_NEW_LANG:
        new_lang_callback_list.erase((void(*)(Language*))callback);
        break;
    default:
        break;
    }
}

}// namespace langmanager
