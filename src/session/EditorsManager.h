#ifndef EDITORSMANAGER_H_
#define EDITORSMANAGER_H_

#include "editarea/EditArea.h"
#include "src/filemanagement/FileTree.h"
#include "toolset/event/Event.h"

#include <string>
#include <vector>

typedef struct {
    EventReceiver* receiver;
    void (EventReceiver::*callback)(EditArea*);
} cb_NewEditArea;

// forward declare
typedef struct AppContext AppContext ;

class EditorsManager : public EventReceiver {
public:
    enum Signal {
        EDITAREA_CREATED
    };

    EditorsManager(AppContext& context);
    ~EditorsManager();

    EditArea& EditNewFile();
    EditArea& EditFile(FileBranch& file);

    EditArea* FindEditAreaByPath(const std::string& path);

    EditArea* GetFocused();

    void ConnectEditAreaCreated(EventReceiver* receiver, void (EventReceiver::*callback)(EditArea*));

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);


private:
    EditArea& CreateEditArea(FileBranch& file);

    AppContext& m_context;

    EditArea* m_focused;

    std::unordered_map<Signal, SimpleEvent> m_eventMap = {
        {EDITAREA_CREATED, SimpleEvent()},
    };

    std::vector<cb_NewEditArea*> m_newEitareaCbs;
};

#endif
