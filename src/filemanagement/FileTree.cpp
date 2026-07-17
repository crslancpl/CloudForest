#include "FileTree.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"
#include "toolset/tools/Tool.h"

#include <algorithm>
#include <memory>
#include <string>

Branch::Branch(std::unique_ptr<FileData> data) : m_fileData(std::move(data)){
    m_name = m_fileData->fileName;
}

const char* Branch::GetName(){
    return m_name;
}

FolderBranch* Branch::GetParent(){
    return m_parent;
}

FileData* Branch::GetFileData(){
    return m_fileData.get();
}

/*
 * FileBranch
 */

FileBranch::FileBranch(std::unique_ptr<FileData> file) : Branch(std::move(file)){
    //
}

void FileBranch::SetParent(FolderBranch* parent){
    std::unique_ptr<FileBranch> thisuniqueptr = m_parent->MoveChildFile(this);
    parent->AddChildFile(std::move(thisuniqueptr));
    m_parent = parent;
}


/*
 * FolderBranch
 */

bool FolderBranch::GetIsChildLoaded(){
    return m_isChildLoaded;
}

void FolderBranch::SetIsChildLoaded(bool loaded){
    m_isChildLoaded = loaded;
}

const std::vector<std::unique_ptr<FileBranch>> &FolderBranch::GetChildFiles(){
    return m_childFiles;
}

const std::vector<std::unique_ptr<FolderBranch>> &FolderBranch::GetChildFolders(){
    return m_childFolders;
}

FolderBranch::FolderBranch(std::unique_ptr<FileData> folder) : Branch(std::move(folder)){
    //
}

void FolderBranch::AddChildFile(std::unique_ptr<FileBranch> child){
    if (!child) return;

    int itr = 0;
    for (std::unique_ptr<FileBranch>& item : m_childFiles) {
        if (item->GetFileData()->sortingCode1 > child->GetFileData()->sortingCode1) {
            break;
        }
        itr++;
    }
    m_childFiles.insert(m_childFiles.begin() + itr, std::move(child));
}

std::unique_ptr<FileBranch> FolderBranch::MoveChildFile(FileBranch* child){
    if (!child) return nullptr;

    for (std::unique_ptr<FileBranch>& item : m_childFiles) {
        if (item.get() == child) {
            return std::move(item);
        }
    }
    return nullptr;
}

void FolderBranch::RemoveChildFile(FileBranch* child){
    //m_childFiles.erase(std::find(m_childFiles.begin(), m_childFiles.end(), child));
}

void FolderBranch::AddChildFolder(std::unique_ptr<FolderBranch> child){
    if (!child) return;

    int itr = 0;
    for (std::unique_ptr<FolderBranch>& item : m_childFolders) {
        if (item->GetFileData()->sortingCode1 > child->GetFileData()->sortingCode1) {
            break;
        }
        itr++;
    }

    m_childFolders.insert(m_childFolders.begin() + itr, std::move(child));
}

std::unique_ptr<FolderBranch> FolderBranch::MoveChildFolder(FolderBranch* child){
    if (!child) return nullptr;

    for (std::unique_ptr<FolderBranch>& item : m_childFolders) {
        if (item.get() == child) {
            return std::move(item);
        }
    }

    return nullptr;
}

void FolderBranch::RemoveChildFolder(FolderBranch* child){
    //m_childFolders.erase(std::find(m_childFolders.begin(), m_childFolders.end(), child));
}

void FolderBranch::SetParent(FolderBranch* parent){
    std::unique_ptr<FolderBranch> thisuniqueptr = m_parent->MoveChildFolder(this);
    parent->AddChildFolder(std::move(thisuniqueptr));
    m_parent = parent;
}

/*
 * Workspace
 */

Workspace::Workspace(std::unique_ptr<FileData> root) : FolderBranch(std::move(root)){
    //
}

FileData* Workspace::FindChildByPath(const char* path){
    // path = "/home/my/path/to/file.txt"
    // workspace path = "/home/my"

    std::string relativepath = std::string(path).substr(strlen(m_fileData->absoPath));
    // relative path = "/path/to/file.txt"

    auto trimmed = tools::TrimText(relativepath, "/");
    // trimmed = ["path", "to", "file.txt"]

    FolderBranch* parent = this;
    for (int i = 0; i < trimmed.size(); i++) {
        std::string& str = trimmed[i];
        for (const std::unique_ptr<FolderBranch>& b : parent->GetChildFolders()) {
            if(b->GetName() == str){
                if (i == trimmed.size() - 1) {
                    // target
                    return b->GetFileData();
                }
                parent = b.get();
                break;
            }
        }

        for (const std::unique_ptr<FileBranch>& b : parent->GetChildFiles()) {
            if (b->GetName() == str) {
                return b->GetFileData();
            }
        }
    }

    return nullptr;
}

Branch* Workspace::FileChildBranchByPath(const char* path){
    // path = "/home/my/path/to/file.txt"
    // workspace path = "/home/my"

    std::string relativepath = std::string(path).substr(strlen(m_fileData->absoPath));
    // relative path = "/path/to/file.txt"

    auto trimmed = tools::TrimText(relativepath, "/");
    // trimmed = ["path", "to", "file.txt"]

    FolderBranch* parent = this;
    for (int i = 0; i < trimmed.size(); i++) {
        std::string &str = trimmed[i];
        for (const std::unique_ptr<FolderBranch>& b : parent->GetChildFolders()) {
            if(b->GetName() == str){
                //printf("found folder %s\n", b->GetName());
                if (i == trimmed.size() - 1) {
                    // target
                    return b.get();
                }
                parent = b.get();
                break;
            }
        }

        for (const std::unique_ptr<FileBranch>& b : parent->GetChildFiles()) {
            if (b->GetName() == str) {
                return b.get();
            }
        }
    }
    return nullptr;
}

void Workspace::SetCustomName(const char* name){
    m_customName = strdup(name);
}
