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
        EFirstUsed.Invoke(this);
        m_used = true;
    }

    EEditAreaAdded.Invoke(ea);
}

void Language::Remove(EditArea* ea){
    m_editareas.erase(ea);
    EEditAreaRemoved.Invoke(ea);
}

const LanguageInfo* Language::GetInfo() const{
    return m_langInfo.get();
}

const std::unordered_set<EditArea*>& Language::GetEditAreas() const{
    return m_editareas;
}
