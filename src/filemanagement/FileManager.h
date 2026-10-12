#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileTree.h"
#include "toolset/event/Event.h"
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>
#include <vector>

// forward declare
typedef struct AppContext AppContext;
typedef struct FileData FileData;

typedef struct {
    EventReceiver* receiver;
    void (EventReceiver::*callback)(FileData*);
} cb_SingleFileOpened;

typedef struct {
    EventReceiver* receiver;
    void (EventReceiver::*callback)(Workspace*);
} cb_WorkspaceCreated;


class FileManager : EventReceiver{
public:
    FileManager(AppContext &context);
    ~FileManager();

    enum Signal{
        NEW_WORKSPACE
    };


    FileBranch& CreateFile(const char* name, FolderBranch* parent);
    FileBranch& CreateNewFile();
    std::unique_ptr<FileBranch> CreateVirtualFile();
    std::unique_ptr<FileBranch> CreateVirtualFolder();

    void ChooseFile();
    void ChooseFolder();

    FileBranch* FindFileByPath(const std::string& name);
    Workspace* FindWorkspace(const FileBranch& filebranch);
    Workspace* FindWorkspaceByPath(const std::string& path);

    std::vector<std::unique_ptr<Workspace>> &GetWorkspaces();

    void ConnectWorkspaceCreated(EventReceiver* receiver, void (EventReceiver::*callback)(Workspace*));

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);

    //callbacks for GTK events
    void OnNewFileSelected(GFile* file);
    void OnNewFolderSelected(GFile* file);

private:
    AppContext& m_context;

    std::vector<std::unique_ptr<Workspace>> m_workspaces;
    std::vector<std::unique_ptr<FileBranch>> m_singleFiles;

    std::vector<cb_SingleFileOpened*> m_singleFileOpenedCbs;
    std::vector<cb_WorkspaceCreated*> m_workspaceCreatedCbs;

    std::unordered_map<Signal, SimpleEvent> eventMap = {
        {NEW_WORKSPACE, SimpleEvent()}
    };
};

#endif
