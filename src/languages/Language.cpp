#include "Language.h"

#include "datatypes/common.h"
#include "pythonbackend/language/language_class_Py.h"
#include "toolset/event/Event.h"

#include <memory>

Language::Language(std::unique_ptr<LanguageInfo> lang) :
    m_langInfo(std::move(lang))
{
    m_py_lang = py_Language_create_object(this);
}

Language::~Language(){}

void Language::Add(EditArea* ea){
    m_editareas.emplace(ea);
    if (!m_used) {
        EFirstUsed.Invoke(this);
        m_used = true;
    }

    const EditAreaAddedArgs args {
        this,
        ea
    };
    EEditAreaAdded.Invoke(args);
}

void Language::Remove(EditArea* ea){
    m_editareas.erase(ea);

    const EditAreaRemovedArgs args {
        this,
        ea
    };

    EEditAreaRemoved.Invoke(args);
}

py_Language* Language::GetPyLanguage() const{
    return m_py_lang;
}

const LanguageInfo* Language::GetInfo() const{
    return m_langInfo.get();
}

const std::unordered_set<EditArea*>& Language::GetEditAreas() const{
    return m_editareas;
}
