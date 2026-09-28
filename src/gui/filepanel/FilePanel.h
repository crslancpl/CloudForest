#ifndef FILEPANEL_H_
#define FILEPANEL_H_

#include "components/CfComponent.h"
#include "components/CfContent.h"
#include "toolset/event/Event.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <vector>

typedef struct FileData FileData;
typedef struct Workspace Workspace;
typedef struct AppContext AppContext;

class FPFileButton;
class FPFolderButton;

class WorkspaceBox: public CfComponent{
public:
    WorkspaceBox(Workspace& ws, AppContext& context);
    ~WorkspaceBox();

    void SetName(const char* name);

    GtkWidget* GetBaseWidget() override;

private:
    Workspace& m_ws;
    std::unique_ptr<FPFolderButton> m_folderBtn;

    GtkBox *m_box;
    GtkLabel *m_label;
};

class FilePanel : public CfContent, public EventReceiver{
public:
    FilePanel(AppContext& appui);
    ~FilePanel();

    static unsigned short Offset;
    // Space in front of the buttons in FilePanel. OffSet * level is the total space

private:
    void OnWorkspaceCreated(Workspace* ws);

    AppContext& m_context;
    GtkBox *m_workspaceArea;
    std::vector<std::unique_ptr<WorkspaceBox>> m_workspaceList;
};



namespace filepanel{
    FPFolderButton *GetFileButton(FileData *filedata);
}

#endif
