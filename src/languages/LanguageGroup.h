#ifndef LANGUAGEGROUP_H_
#define LANGUAGEGROUP_H_

#include "datatypes/common.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"

#include <unordered_map>
#include <unordered_set>

typedef void (*LanguageGroupBasicCallback)(EditArea*);

/*
 * Grouping EditArea by language
 */

class LanguageGroup{
public:
    enum Signal{
        ADDED,
        REMOVED
    };

    LanguageGroup(const Language* lang);

    void Add(EditArea* ea);
    void Remove(EditArea* ea);
    const std::unordered_set<EditArea*>& GetEditAreaSet();

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);

private:
    const Language* m_language;
    std::unordered_set<EditArea*> m_editareaSet;
    std::unordered_map<Signal, SimpleEvent> m_eventMap = {
        {ADDED, SimpleEvent()},
        {REMOVED, SimpleEvent()}
    };
};

#endif
