#include "LanguageListener.h"

#include "LanguageManager_if.h"

#include <unordered_map>
#include <unordered_set>

class LangCallback{
public:
    LangCallback(datatypes::Language* lang){
        m_lang = lang;
    }

    void InsertCallback(void(*callback)(EditArea*) ){
        m_callbacks.insert(callback);
    }

    void Run(EditArea* ea){
        for(auto cb : m_callbacks){
            cb(ea);
        }
    }

private:
    datatypes::Language *m_lang;
    std::unordered_set<void(*)(EditArea*)> m_callbacks = {};
};

static std::unordered_map<datatypes::Language*, LangCallback*> lang_and_callbacks_map;

void LanguageNewEditArea(EditArea* ea, datatypes::Language* lang){
    auto l = lang_and_callbacks_map.find(lang);
    LangCallback* langcallback;
    if(l == lang_and_callbacks_map.end()){
        langcallback = new LangCallback(lang);
        lang_and_callbacks_map.emplace(lang, langcallback);
    }else{
        langcallback = l->second;
    }

   langcallback->Run(ea);
}

void ListenNewEditAreaForLanguage(const char* langname, void (*callback)(EditArea*)){
    const auto lang = langmanager::FindLanguage(langname);
    if(!lang){
        return;
    }

    auto l = lang_and_callbacks_map.find(lang);
    LangCallback* langcallback;
    if(l == lang_and_callbacks_map.end()){
        langcallback = new LangCallback(lang);
        lang_and_callbacks_map.emplace(lang, langcallback);
    }else{
        langcallback = l->second;
    }

    langcallback->InsertCallback(callback);
}
