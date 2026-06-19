#include "LanguageListener.h"

#include "LanguageManager_if.h"
#include "datatypes/common.h"
#include "pythonbackend/language_mod_Py.h"
#include "src/gui/editarea/EditArea.h"
#include "src/session/SessionEvent.h"
#include "toolset/event/Event.h"

namespace langmanager{

typedef void (*AddEditAreaCallback)(const char*, const EditArea*);

class AddEditAreaEvemt : public SimpleEvent{
public:
    AddEditAreaEvemt(const Language* lang){
        m_lang = lang;
    }

    void Run(EditArea* ea){
        for(EventCallback callback : m_callbackSet){
            ((AddEditAreaCallback)callback)(m_lang->name, ea);
        }
    }

private:
    const Language *m_lang;
};

static std::unordered_map<Language*, AddEditAreaEvemt> lang_and_callbacks_map;


static void UpdateTextAreaLanguage(TextArea* ta, Language* lang){
    // the callback for EditArea->SetLanguage
    language_module_invoke_new_editarea(lang->name, (EditArea*)ta);
}

static void OnEditAreaCreated(EditArea* ea){
    // pass to editarea::AddNewEditAreaCallback()
    ea->Listen(EditArea::LANG_CHANGED, (EventCallback)UpdateTextAreaLanguage);
    language_module_invoke_new_editarea(ea->GetLanguage()->name, ea);
}

void AddEditArea(EditArea* ea, Language* lang){
    auto itr = lang_and_callbacks_map.find(lang);

    if(itr != lang_and_callbacks_map.end()){
        AddEditAreaEvemt &event = itr->second;
        event.Run(ea);
    }else{
        auto pair = lang_and_callbacks_map.emplace(lang, AddEditAreaEvemt(lang));
        AddEditAreaEvemt &event = pair.first->second;
        event.Run(ea);
    }
}

void ListenNewEditArea(const char* langname, void (*callback)(const char*, EditArea*)){
    const auto lang = langmanager::FindByName(langname);
    if(!lang){
        return;
    }

    auto itr = lang_and_callbacks_map.find(lang);
    if(itr != lang_and_callbacks_map.end()){
        AddEditAreaEvemt &event = itr->second;
        event.Connect((EventCallback)callback);
    }else{
        auto pair = lang_and_callbacks_map.emplace(lang, AddEditAreaEvemt(lang));
        AddEditAreaEvemt &event = pair.first->second;
        event.Connect((EventCallback)callback);
    }
}

void StartListener(){
    session::Listen(session::EDITAREA_CREATED, (EventCallback)OnEditAreaCreated);
}

} //namepsace langmanager
