#ifndef LANGUAGEGROUP_H_
#define LANGUAGEGROUP_H_

#include "components/TextArea.h"
#include "datatypes/common.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"

#include <memory>
#include <unordered_set>

/*
 * Grouping EditArea by language
 */

class Language {
public:
    Language(std::unique_ptr<LanguageInfo> lang);
    ~Language();

    void Add(EditArea* ea);
    void Remove(EditArea* ea);

    const LanguageInfo* GetInfo() const;
    const std::unordered_set<EditArea*>& GetEditAreas() const;

    Event<Language*> EFirstUsed;
    Event<EditArea*> EEditAreaAdded;
    Event<EditArea*> EEditAreaRemoved;

private:
    bool m_used = false;

    std::unique_ptr<LanguageInfo> m_langInfo;
    std::unordered_set<EditArea*> m_editareas;
};

#endif
