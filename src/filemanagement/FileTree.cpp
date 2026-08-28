#include "FileTree.h"

#include "datatypes/file.h"
#include "src/filemanagement/FileOperation.h"
#include "src/filemanagement/FileReader.h"
#include "toolset/tools/StringSorting.h"
#include "toolset/tools/Tool.h"

#include <memory>
#include <string>

Branch::Branch(std::unique_ptr<FileData> data) : m_fileData(std::move(data)){
    m_name = m_fileData->fileName;
    m_fileData->branch = this;
}

void Branch::Rename(const std::string& name){
    filemanager::Rename(m_fileData.get(), name);
}

void Branch::Delete(){
    filemanager::Delete(m_fileData.get());
}

const std::string& Branch::GetName() const{
    return m_name;
}

FolderBranch* Branch::GetParent() const{
    return m_parent;
}

void Branch::SetParent(FolderBranch* parent){
    m_parent = parent;
}

FileData* Branch::GetFileData() const{
    return m_fileData.get();
}

/*
 * FileBranch class
 */

FileBranch::FileBranch(std::unique_ptr<FileData> file) : Branch(std::move(file)){
    //
}

FileBranch::~FileBranch(){}

const char* FileBranch::ReadContent(){
    char* content;
    filemanager::ReadFileText(m_fileData.get(), &content);
    return content;
}

/*
 * FolderBranch class
 */

FolderBranch::FolderBranch(std::unique_ptr<FileData> folder) : Branch(std::move(folder)){
    //
}

FolderBranch::~FolderBranch(){}

void FolderBranch::Load(){
    if(m_isChildLoaded){
        // already expanded
        return;
    }

    if (m_fileData->isVirtual) {
        m_isChildLoaded = true;
        return;
    }

    GFileEnumerator *enumerator = g_file_enumerate_children(m_fileData->file, "*", G_FILE_QUERY_INFO_NONE,
        nullptr, nullptr);

    GFileInfo* info;

    while ((info = g_file_enumerator_next_file(enumerator, nullptr, nullptr)) != nullptr) {
        GFile *child = g_file_enumerator_get_child(enumerator, info);
        std::unique_ptr<FileData> data = filemanager::LoadFileData(child, info, false);
        if (child == nullptr|| data->fileInfo == nullptr) continue;

        if (data->type == G_FILE_TYPE_REGULAR){
            std::unique_ptr<FileBranch> b = std::make_unique<FileBranch>(std::move(data));
            this->AddChildFile(std::move(b));
        } else if (data->type == G_FILE_TYPE_DIRECTORY){
            std::unique_ptr<FolderBranch> b = std::make_unique<FolderBranch>(std::move(data));
            this->AddChildFolder(std::move(b));
        }
    }
    m_isChildLoaded = true;
}

bool FolderBranch::GetIsChildLoaded() const{
    return m_isChildLoaded;
}

const std::vector<std::unique_ptr<FileBranch>> &FolderBranch::GetChildFiles() const{
    return m_childFiles;
}

const std::vector<std::unique_ptr<FolderBranch>> &FolderBranch::GetChildFolders() const{
    return m_childFolders;
}

void FolderBranch::AddChildFile(std::unique_ptr<FileBranch> child){
    if (!child) return;

    int itr = 0;
    for (std::unique_ptr<FileBranch>& item : m_childFiles) {
        if (CompareString(child->GetName(), item->GetName()) == COMPARE_RESULT_LESS) {
            break;
        }

        itr++;
    }

    child->SetParent(this);
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
    int itr = 0;
    for (const std::unique_ptr<FileBranch>& item : m_childFiles) {
        if (item.get() == child) {
            m_childFiles.erase(m_childFiles.begin() + itr);
        }
        itr ++;
    }
}

void FolderBranch::AddChildFolder(std::unique_ptr<FolderBranch> child){
    if (!child) return;

    int itr = 0;
    for (std::unique_ptr<FolderBranch>& item : m_childFolders) {
        if (CompareString(child->GetName(), item->GetName()) == COMPARE_RESULT_LESS) {
            break;
        }

        itr++;
    }

    child->SetParent(this);
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

/*
 * Workspace
 */

Workspace::Workspace(std::unique_ptr<FileData> root) : FolderBranch(std::move(root)){
    //
}

FileData* Workspace::FindChildByPath(const char* path){
    // path = "/home/my/path/to/file.txt"
    // workspace path = "/home/my"

    std::string relativepath = std::string(path).substr(m_fileData->absoPath.length());
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

Branch* Workspace::FindChildBranchByPath(const char* path){
    // path = "/home/my/path/to/file.txt"
    // workspace path = "/home/my"

    std::string relativepath = std::string(path).substr(m_fileData->absoPath.length());
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
