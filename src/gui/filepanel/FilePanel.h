#ifndef FILEPANEL_H_
#define FILEPANEL_H_

#include "src/gui/components/CfComponent.h"
#include "src/gui/components/CfContent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

typedef struct FileData FileData;
typedef struct Workspace Workspace;

class FPFileButton;
class FPFolderButton;

class WorkspaceBox: public CfComponent{
public:
    WorkspaceBox(Workspace* ws);
    void SetName(const char* name);

    GtkWidget* GetBaseWidget() override;
private:
    Workspace* m_ws;
    GtkBox *m_box;
    GtkLabel *m_label;
};

class FilePanel : public CfContent{
public:
    FilePanel();

    static unsigned short Offset;
    // Space in front of the buttons in FilePanel. OffSet * level is the total space
    void NewWorkspace(Workspace* ws);

private:
    GtkBox *m_workspaceArea;
    std::unordered_set<WorkspaceBox*> m_workspaceList;
    void AddWorkspace(Workspace* ws);

};



namespace filepanel{
    FPFolderButton *GetFileButton(FileData *filedata);
}

#endif
