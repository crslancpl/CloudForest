#ifndef LANGUAGEGROUP_H_
#define LANGUAGEGROUP_H_

#include "components/TextArea.h"
#include "datatypes/common.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"

#include <memory>
#include <unordered_set>


struct py_Language ;

/*
 * Grouping EditArea by language
 */

class Language {
public:
    typedef struct {
        Language* lang;
        EditArea* ea;
    }EditAreaAddedArgs ;

    typedef struct {
        Language* lang;
        EditArea* ea;
    }EditAreaRemovedArgs ;

    Language(std::unique_ptr<LanguageInfo> lang);
    ~Language();

    void Add(EditArea* ea);
    void Remove(EditArea* ea);

    py_Language* GetPyLanguage() const;
    const LanguageInfo* GetInfo() const;
    const std::unordered_set<EditArea*>& GetEditAreas() const;

    Event<Language*> EFirstUsed;
    Event<const EditAreaAddedArgs&> EEditAreaAdded;
    Event<const EditAreaRemovedArgs&> EEditAreaRemoved;

private:
    bool m_used = false;

    py_Language* m_py_lang = nullptr;

    std::unique_ptr<LanguageInfo> m_langInfo;
    std::unordered_set<EditArea*> m_editareas;
};

#endif
