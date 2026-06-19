#ifndef FILETREE_H_
#define FILETREE_H_

#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"

#include <vector>

/*
 * Forward declare
 */
class Branch;
class FileBranch;
class FolderBranch;
class Workspace;

class Branch{
public:
    Branch(FileData* file);

    const char* GetName();

    virtual FolderBranch* GetParent();
    virtual void SetParent(FolderBranch* parent) = 0;
    virtual FileData* GetFileData();

protected:
    const char* m_name;
    FileData* m_fileData;
    FolderBranch* m_parent;
};



class FileBranch : public Branch{
public:
    FileBranch(FileData* file);

    void SetParent(FolderBranch* parent) override;
};



class FolderBranch : public Branch{
public:
    FolderBranch(FileData* folder);

    bool GetIsChildLoaded();
    void SetIsChildLoaded(bool loaded);
    const std::vector<FileBranch*> &GetChildFiles();
    const std::vector<FolderBranch*> &GetChildFolders();
    void AddChildFile(FileBranch* child);
    void RemoveChildFile(FileBranch* child);
    void AddChildFolder(FolderBranch* child);
    void RemoveChildFolder(FolderBranch* child);

    void SetParent(FolderBranch* parent) override;

protected:
    bool m_isChildLoaded = false;
    std::vector<FileBranch*> m_childFiles;
    std::vector<FolderBranch*> m_childFolders;
};



class Workspace : public FolderBranch{
public:
    Workspace(FileData* root);

    FileData* FindChildByPath(const char* path);
    Branch* FileChildBranchByPath(const char* path);
    void SetCustomName(const char* name);
private:
    char* m_customName;// can be named by user
};


#endif
