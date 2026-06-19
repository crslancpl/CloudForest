#include "FileTree.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileReader.h"
#include "toolset/tools/Tool.h"

#include <algorithm>
#include <cstdio>
#include <string>

Branch::Branch(FileData* data) : m_fileData(data){
    m_name = m_fileData->fileName;
}

const char* Branch::GetName(){
    return m_name;
}

FolderBranch* Branch::GetParent(){
    return m_parent;
}

FileData* Branch::GetFileData(){
    return m_fileData;
}

/*
 * FileBranch
 */

FileBranch::FileBranch(FileData* file) : Branch(file){
    //
}

void FileBranch::SetParent(FolderBranch* parent){
    m_parent->RemoveChildFile(this);
    parent->AddChildFile(this);
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

const std::vector<FileBranch*> &FolderBranch::GetChildFiles(){
    return m_childFiles;
}

const std::vector<FolderBranch*> &FolderBranch::GetChildFolders(){
    return m_childFolders;
}

FolderBranch::FolderBranch(FileData* folder) : Branch(folder){
    //
}

void FolderBranch::AddChildFile(FileBranch* child){
    if (!child) return;

    int itr = 0;
    for (FileBranch* item : m_childFiles) {
        if (item->GetFileData()->sortingCode1 > child->GetFileData()->sortingCode1) {
            break;
        }
        itr++;
    }
    m_childFiles.insert(m_childFiles.begin() + itr, child);
}

void FolderBranch::RemoveChildFile(FileBranch* child){
    m_childFiles.erase(std::find(m_childFiles.begin(), m_childFiles.end(), child));
}

void FolderBranch::AddChildFolder(FolderBranch* child){
    if (!child) return;

    int itr = 0;
    for (FolderBranch* item : m_childFolders) {
        if (item->GetFileData()->sortingCode1 > child->GetFileData()->sortingCode1) {
            break;
        }
        itr++;
    }

    m_childFolders.insert(m_childFolders.begin() + itr, child);
}

void FolderBranch::RemoveChildFolder(FolderBranch* child){
    m_childFolders.erase(std::find(m_childFolders.begin(), m_childFolders.end(), child));
}

void FolderBranch::SetParent(FolderBranch* parent){
    m_parent->RemoveChildFolder(this);
    parent->AddChildFolder(this);
    m_parent = parent;
}

/*
 * Workspace
 */

Workspace::Workspace(FileData* root) : FolderBranch(root){
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
        for (FolderBranch* b : parent->GetChildFolders()) {
            if(b->GetName() == str){
                if (i == trimmed.size() - 1) {
                    // target
                    return b->GetFileData();
                }
                parent = b;
                break;
            }
        }

        for (FileBranch* b : parent->GetChildFiles()) {
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
        for (FolderBranch* b : parent->GetChildFolders()) {
            if(b->GetName() == str){
                //printf("found folder %s\n", b->GetName());
                if (i == trimmed.size() - 1) {
                    // target
                    return b;
                }
                parent = b;
                break;
            }
        }

        for (FileBranch* b : parent->GetChildFiles()) {
            if (b->GetName() == str) {
                return b;
            }
        }
    }
    return nullptr;
}

void Workspace::SetCustomName(const char* name){
    m_customName = strdup(name);
}
