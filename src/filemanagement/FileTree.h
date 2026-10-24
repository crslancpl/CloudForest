#ifndef FILETREE_H_
#define FILETREE_H_

#include "datatypes/file.h"
#include "editarea/EditArea.h"
#include "src/filemanagement/FileReader.h"
#include "toolset/event/Event.h"

#include <memory>
#include <string>
#include <vector>

/*
 * Forward declare
 */
class Branch;
class FileBranch;
class FolderBranch;
class Workspace;
class FPFileButton;
class FPFolderButton;
typedef struct AppContext AppContext;

class Branch {
public:
    Branch(std::unique_ptr<FileData> file);

    void Rename(const std::string& name);
    void Delete();

    // callbacks

    virtual FolderBranch* GetParent() const;
    virtual void SetParent(FolderBranch* parent);
    virtual const FileData* GetFileData() const;

    FilePanelButton* GetFilePanelButton();
    void SetFilePanelButton(FilePanelButton& button);

protected:
    std::unique_ptr<FileData> m_fileData;
    FolderBranch* m_parent;
    FilePanelButton* m_button;
};



class FileBranch : public Branch{
public:
    FileBranch(std::unique_ptr<FileData> file);
    ~FileBranch();

    void Save(const char* content, AppContext& context);
    const char* ReadContent();

    EditArea* GetEditArea() const;
    void SetEditArea(EditArea* editarea);

    Event<FileBranch*> EFocused;
    Event<FileBranch*> EOpened;
    Event<FileBranch*> ESaved;
    Event<FileBranch*> EClosed;
    Event<FileBranch*> EDataChanged;

    void OnSaved(std::unique_ptr<FileData> filedata);

private:
    EditArea* m_editarea = nullptr;

    const char* m_latestContent;
    FPFileButton* m_filePanelButton = nullptr;
};



class FolderBranch : public Branch{
public:
    FolderBranch(std::unique_ptr<FileData> folder);
    ~FolderBranch();

    void Load();

    bool GetIsChildLoaded() const;

    const std::vector<std::unique_ptr<FileBranch>> &GetChildFiles() const;
    const std::vector<std::unique_ptr<FolderBranch>> &GetChildFolders() const;

    void AddChildFile(std::unique_ptr<FileBranch> child);
    std::unique_ptr<FileBranch> MoveChildFile(FileBranch* child);
    void RemoveChildFile(FileBranch* child);

    void AddChildFolder(std::unique_ptr<FolderBranch> child);
    std::unique_ptr<FolderBranch> MoveChildFolder(FolderBranch* child);
    void RemoveChildFolder(FolderBranch* child);

protected:
    bool m_isChildLoaded = false;

    std::vector<std::unique_ptr<FileBranch>> m_childFiles;
    std::vector<std::unique_ptr<FolderBranch>> m_childFolders;
};



class Workspace : public FolderBranch{
public:
    Workspace(std::unique_ptr<FileData> root);

    Branch* FindChildBranchByPath(const std::string& path);
    void SetCustomName(const std::string& name);
private:
    std::string m_customName;// can be named by user
};


#endif
