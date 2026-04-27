#include "LanguageListener.h"

#include "LanguageManager_if.h"
#include "datatypes/language.h"
#include "pythonbackend/language_mod_Py.h"
#include "src/gui/editarea/EditArea.h"
#include "src/gui/editarea/EditArea_if.h"


#include <unordered_map>
#include <unordered_set>

class LangCallback{
public:
    LangCallback(Language* lang){
        m_lang = lang;
    }

    void InsertCallback(void(*callback)(const char*, EditArea*) ){
        m_callbacks.insert(callback);
    }

    void Run(EditArea* ea){
        for(auto cb : m_callbacks){
            cb(m_lang->name.c_str(), ea);
        }
    }

private:
    Language *m_lang;
    std::unordered_set<void(*)(const char*, EditArea*)> m_callbacks = {};
};

static std::unordered_map<Language*, LangCallback*> lang_and_callbacks_map;


static void UpdateTextAreaLanguage(TextArea* ta, Language* lang){
    // the callback for EditArea->SetLanguage
    language_module_invoke_new_editarea(lang->name.c_str(), (EditArea*)ta);
}

static void EditAreaCreated(EditArea* ea){
    // pass to editarea::AddNewEditAreaCallback()
    ea->ListenEvent(EditArea::TEXTAREA_CLASS_LANG_CHANGED, (void(*)())UpdateTextAreaLanguage);
    language_module_invoke_new_editarea(ea->GetLanguage()->name.c_str(), ea);
}

void LanguageNewEditArea(EditArea* ea, Language* lang){
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

void ListenNewEditAreaForLanguage(const char* langname, void (*callback)(const char*, EditArea*)){
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

void LanguageListenerStart(){
    editarea::ListenEvent(editarea::EDITAREA_CREATED, (void (*)())EditAreaCreated);
}
