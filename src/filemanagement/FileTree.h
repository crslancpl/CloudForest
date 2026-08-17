#ifndef FILETREE_H_
#define FILETREE_H_

#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"

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

class Branch{
public:
    Branch(std::unique_ptr<FileData> file);

    const std::string& GetName() const;

    virtual FolderBranch* GetParent() const;
    virtual void SetParent(FolderBranch* parent) = 0;
    virtual FileData* GetFileData() const;

protected:
    std::string m_name;
    std::unique_ptr<FileData> m_fileData;
    FolderBranch* m_parent;
};



class FileBranch : public Branch{
public:
    FileBranch(std::unique_ptr<FileData> file);

    void SetParent(FolderBranch* parent) override;
};



class FolderBranch : public Branch{
public:
    FolderBranch(std::unique_ptr<FileData> folder);

    bool GetIsChildLoaded() const;
    void SetIsChildLoaded(bool loaded);
    const std::vector<std::unique_ptr<FileBranch>> &GetChildFiles() const;
    const std::vector<std::unique_ptr<FolderBranch>> &GetChildFolders() const;

    void AddChildFile(std::unique_ptr<FileBranch> child);
    std::unique_ptr<FileBranch> MoveChildFile(FileBranch* child);
    void RemoveChildFile(FileBranch* child);

    void AddChildFolder(std::unique_ptr<FolderBranch> child);
    std::unique_ptr<FolderBranch> MoveChildFolder(FolderBranch* child);
    void RemoveChildFolder(FolderBranch* child);

    void SetParent(FolderBranch* parent) override;

protected:
    bool m_isChildLoaded = false;
    std::vector<std::unique_ptr<FileBranch>> m_childFiles;
    std::vector<std::unique_ptr<FolderBranch>> m_childFolders;
};



class Workspace : public FolderBranch{
public:
    Workspace(std::unique_ptr<FileData> root);

    FileData* FindChildByPath(const char* path);
    Branch* FileChildBranchByPath(const char* path);
    void SetCustomName(const char* name);
private:
    std::string m_customName;// can be named by user
};


#endif
