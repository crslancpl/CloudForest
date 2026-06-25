#include "LanguageGroup.h"
#include "toolset/event/Event.h"

LanguageGroup::LanguageGroup(const Language* lang) : m_language(lang){
    //
}

void LanguageGroup::Add(EditArea* ea){
    m_editareaSet.emplace(ea);
    SimpleEvent& event = m_eventMap.at(ADDED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LanguageGroupBasicCallback)callback)(ea);
    }
}

void LanguageGroup::Remove(EditArea* ea){
    m_editareaSet.erase(ea);
    SimpleEvent& event = m_eventMap.at(REMOVED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((LanguageGroupBasicCallback)callback)(ea);
    }
}

const std::unordered_set<EditArea*>& LanguageGroup::GetEditAreaSet(){
    return m_editareaSet;
}

void LanguageGroup::Listen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if(itr != m_eventMap.end()){
        itr->second.Connect(callback);
    }
}

void LanguageGroup::StopListen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if(itr != m_eventMap.end()){
        itr->second.Disconnect(callback);
    }
}
