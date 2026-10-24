#ifndef EDITORSMANAGER_H_
#define EDITORSMANAGER_H_

#include "editarea/EditArea.h"
#include "src/filemanagement/FileTree.h"
#include "toolset/event/Event.h"

#include <string>
// forward declare
typedef struct AppContext AppContext ;

class EditorsManager : public EventReceiver {
public:
    EditorsManager(AppContext& context);
    ~EditorsManager();

    EditArea& EditNewFile();
    EditArea& EditFile(FileBranch& file);

    EditArea* FindEditAreaByPath(const std::string& path);

    EditArea* GetFocused();

    Event<EditArea*> EEditAreaCreated;

private:
    EditArea& CreateEditArea(FileBranch& file);

    AppContext& m_context;

    EditArea* m_focused;
};

#endif
