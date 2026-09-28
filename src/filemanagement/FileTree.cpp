#include "FileTree.h"

#include "AppUI.h"
#include "editarea/CompletionTool.h"
#include "editarea/EditArea.h"
#include "src/AppContext.h"
#include "datatypes/file.h"
#include "src/filemanagement/FileManager.h"
#include "src/filemanagement/FileReader.h"
#include "toolset/tools/StringSorting.h"
#include "toolset/tools/Tool.h"
#include "windows/MainWindow.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <memory>
#include <string>

/*
 * Branch class
 */

Branch::Branch(std::unique_ptr<FileData> data) : m_fileData(std::move(data)){
}

void Branch::Rename(const std::string& name){
    m_fileData->fileName = name;
    GError* error;
    GFile* oldfile = m_fileData->file;
    GFile* newfile = g_file_set_display_name(oldfile, name.c_str(), nullptr, &error);
    if (newfile) {
        m_fileData->file = newfile;
        g_object_unref(oldfile);
    }
}

void Branch::Delete(){
    GError* error;
    g_file_delete(m_fileData->file, nullptr, &error);
}

FolderBranch* Branch::GetParent() const{
    return m_parent;
}

void Branch::SetParent(FolderBranch* parent){
    m_parent = parent;
}

const FileData* Branch::GetFileData() const{
    return m_fileData.get();
}

FilePanelButton* Branch::GetFilePanelButton(){
    return m_button;
}
void Branch::SetFilePanelButton(FilePanelButton& button){
    m_button = &button;
}


/*
 * FileBranch class
 */

typedef struct {
    FileData* oldFileData;
    const char* content;
    FileBranch* parent;
} FileSaved_Args;

static void on_new_file_saved(GObject *source, GAsyncResult *result, void* userdata){
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, nullptr);
    FileSaved_Args* args = (FileSaved_Args*)userdata;

    if(file != nullptr){
        GFileInfo* info = g_file_query_info(file, "*", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
        std::unique_ptr<FileData> filedata = filemanager::LoadFileData(file, info, true);

        FileData* filedataraw = filedata.get();
        g_file_replace_contents(
            filedata->file,
            args->content,
            strlen(args->content),
            nullptr,
            false,
            G_FILE_CREATE_REPLACE_DESTINATION,
            nullptr,
            nullptr,
            nullptr
        );

        ((FileBranch*)args->parent)->OnSaved(std::move(filedata));
    }
    delete args;
}

FileBranch::FileBranch(std::unique_ptr<FileData> file) : Branch(std::move(file)){
    //
}

FileBranch::~FileBranch(){}

EditArea* FileBranch::GetEditArea() const{
    return m_editarea;
}

void FileBranch::SetEditArea(EditArea* ea){
    m_editarea = ea;
}

void FileBranch::Save(const char* content, AppContext& context){
    if(m_fileData->isVirtual){
        /*
         * Create a new file or write the content into a existing file.
         */
        FileSaved_Args* args = new FileSaved_Args{
            m_fileData.get(),
            content,
            this
        };// free by on_file_saved()

        gtk_file_dialog_set_title(context.appUI->GetFileDialog(), "Save file");
        MainWindow* parentwindow = context.appUI->GetMainWindow();
        gtk_file_dialog_save(
            context.appUI->GetFileDialog(),
            parentwindow->GetGtkWindow(),
            nullptr,
            on_new_file_saved,
            args
        );
    } else {
        /*
         * Replace content.
         */
        g_file_replace_contents(
            m_fileData->file,
            content,
            strlen(content),
            nullptr,
            false,
            G_FILE_CREATE_REPLACE_DESTINATION,
            nullptr,
            nullptr,
            nullptr);

        m_latestContent = content;

        for (cb_FileBranchBasic* callback : m_savedCallbacks) {
            auto f = callback->funtion;
            auto r = callback->receiver;
            (r->*f)(this);
        }
    }
}

const char* FileBranch::ReadContent(){
    char* content;
    filemanager::ReadFileText(m_fileData.get(), &content);
    return content;
}

void FileBranch::ConnectFocused(EventReceiver* receiver, void (EventReceiver::*callback)(FileBranch*)){
    cb_FileBranchBasic* callbackdata = new cb_FileBranchBasic();
    callbackdata->receiver = receiver;
    callbackdata->funtion = callback;
    m_focusedCallbacks.emplace_back(callbackdata);
}

void FileBranch::ConnectOpened(EventReceiver* receiver, void (EventReceiver::*callback)(FileBranch*)){
    cb_FileBranchBasic* callbackdata = new cb_FileBranchBasic();
    callbackdata->receiver = receiver;
    callbackdata->funtion = callback;
    m_openedCallbacks.emplace_back(callbackdata);
}

void FileBranch::ConnectSaved(EventReceiver* receiver, void (EventReceiver::*callback)(FileBranch*)){
    cb_FileBranchBasic* callbackdata = new cb_FileBranchBasic();
    callbackdata->receiver = receiver;
    callbackdata->funtion = callback;
    m_savedCallbacks.emplace_back(callbackdata);
}

void FileBranch::ConnectClosed(EventReceiver* receiver, void (EventReceiver::*callback)(FileBranch*)){
    cb_FileBranchBasic* callbackdata = new cb_FileBranchBasic();
    callbackdata->receiver = receiver;
    callbackdata->funtion = callback;
    m_closedCallbacks.emplace_back(callbackdata);
}

void FileBranch::ConnectDataChanged(EventReceiver* receiver, void (EventReceiver::*callback)(FileBranch*)){
    cb_FileBranchBasic* callbackdata = new cb_FileBranchBasic();
    callbackdata->receiver = receiver;
    callbackdata->funtion = callback;
    m_dataChangedCallbacks.emplace_back(callbackdata);
}

void FileBranch::OnSaved(std::unique_ptr<FileData> filedata){
    if (filedata->fileName != m_fileData->fileName) {
        //this->Rename(fileName);
    }

    if (filedata.get() != m_fileData.get()) {
        // new file saved
        m_fileData = std::move(filedata);
    }

    for (cb_FileBranchBasic* callbackdata : m_dataChangedCallbacks) {
        auto r = callbackdata->receiver;
        auto f = callbackdata->funtion;
        (r->*f)(this);
    }
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
        if (CompareString(child->GetFileData()->fileName, item->GetFileData()->fileName) == COMPARE_RESULT_LESS) {
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
        if (CompareString(child->GetFileData()->fileName, item->GetFileData()->fileName) == COMPARE_RESULT_LESS) {
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

Branch* Workspace::FindChildBranchByPath(const std::string& path){
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
            if(b->GetFileData()->fileName == str){
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
            if (b->GetFileData()->fileName == str) {
                return b.get();
            }
        }
    }
    return nullptr;
}

void Workspace::SetCustomName(const std::string& name){
    m_customName = name;
}
