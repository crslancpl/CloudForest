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


class FileManager : EventReceiver{
public:
    FileManager(AppContext &context);
    ~FileManager();


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

    Event<Workspace*> EWorkspaceCreated;
    Event<FileData*> ESingleFileOpened;
    //callbacks for GTK events
    void OnNewFileSelected(GFile* file);
    void OnNewFolderSelected(GFile* file);

private:
    AppContext& m_context;

    std::vector<std::unique_ptr<Workspace>> m_workspaces;
    std::vector<std::unique_ptr<FileBranch>> m_singleFiles;
};

#endif
