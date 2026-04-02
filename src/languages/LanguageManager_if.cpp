#include "language_manager_if.h"

#include <unordered_map>
#include <string>
#include "datatypes/language_data.h"

static std::unordered_map<std::string, datatypes::Language> language_list;

void langmanager::Clear(){
    language_list.clear();
}

void langmanager::AddLanguage(datatypes::Language lang){
    language_list.emplace(lang.Name, lang);
}
