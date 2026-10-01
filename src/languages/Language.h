#ifndef LANGUAGEGROUP_H_
#define LANGUAGEGROUP_H_

#include "components/TextArea.h"
#include "datatypes/common.h"
#include "editarea/EditArea.h"
#include "toolset/event/Event.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>

typedef void (*LanguageBasicCallback)(EditArea*);
typedef void (*LanguageUsedCallback)(Language*);
/*
 * Grouping EditArea by language
 */

class Language {
public:
    enum Signal{
        FIRST_EDITAREA_ADDED,
        EDITAREA_ADDED,
        EDITAREA_REMOVED
    };

    Language(std::unique_ptr<LanguageInfo> lang);
    ~Language();

    void Add(EditArea* ea);
    void Remove(EditArea* ea);

    const LanguageInfo* GetInfo() const;
    const std::unordered_set<EditArea*>& GetEditAreas() const;

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);

private:
    bool m_used = false;

    std::unique_ptr<LanguageInfo> m_langInfo;
    std::unordered_set<EditArea*> m_editareas;
    std::unordered_map<Signal, SimpleEvent> m_eventMap = {
        {FIRST_EDITAREA_ADDED, SimpleEvent()},
        {EDITAREA_ADDED, SimpleEvent()},
        {EDITAREA_REMOVED, SimpleEvent()}
    };
};

#endif
