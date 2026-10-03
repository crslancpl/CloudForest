#include "Language.h"
#include "datatypes/common.h"
#include "toolset/event/Event.h"

#include <memory>

Language::Language(std::unique_ptr<LanguageInfo> lang) :
    m_langInfo(std::move(lang))
{
    //
}

Language::~Language(){}

void Language::Add(EditArea* ea){
    m_editareas.emplace(ea);
    if (!m_used) {
        SimpleEvent& event = m_eventMap.at(LANG_FIRST_USED);
        for (EventCallback callback : event.GetCallbackSet()) {
            ((LanguageUsedCallback)callback)(this);
        }
        m_used = true;
    }

    SimpleEvent& event = m_eventMap.at(EDITAREA_ADDED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LanguageBasicCallback)callback)(ea);
    }
}

void Language::Remove(EditArea* ea){
    m_editareas.erase(ea);
    SimpleEvent& event = m_eventMap.at(EDITAREA_REMOVED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LanguageBasicCallback)callback)(ea);
    }
}

const LanguageInfo* Language::GetInfo() const{
    return m_langInfo.get();
}

const std::unordered_set<EditArea*>& Language::GetEditAreas() const{
    return m_editareas;
}

void Language::Listen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if(itr != m_eventMap.end()){
        itr->second.Connect(callback);
    }
}

void Language::StopListen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if(itr != m_eventMap.end()){
        itr->second.Disconnect(callback);
    }
}
