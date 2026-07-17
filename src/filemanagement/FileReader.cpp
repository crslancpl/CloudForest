#include "FileReader.h"

#include "FileTree.h"
#include "datatypes/file.h"
#include "toolset/tools/StringSorting.h"

#include <gio/gio.h>
#include <memory>

/*
 * File Tree:
 * To reduce resource usage, file tree would not read its chile/descendant automatically.
 * Use ExpandFileBranch() to read the child of a branch.
 */

namespace filemanager {

void ExpandFolderBranch(FolderBranch *branch){
    if(branch->GetIsChildLoaded()){
        // already expanded
        return;
    }
    if (branch->GetFileData()->isVirtual) {
        branch->SetIsChildLoaded(true);
        return;
    }

    GFileEnumerator *enumerator = g_file_enumerate_children(branch->GetFileData()->file, "*", G_FILE_QUERY_INFO_NONE,
        nullptr, nullptr);

    GFileInfo* info;

    while ((info = g_file_enumerator_next_file(enumerator, nullptr, nullptr)) != nullptr) {
        GFile *child = g_file_enumerator_get_child(enumerator, info);
        std::unique_ptr<FileData> data = LoadFileData(child, info, false);
        if (child == nullptr|| data->fileInfo == nullptr) continue;

        if (data->type == G_FILE_TYPE_REGULAR){
            std::unique_ptr<FileBranch> b = std::make_unique<FileBranch>(std::move(data));
            branch->AddChildFile(std::move(b));
        } else if (data->type == G_FILE_TYPE_DIRECTORY){
            std::unique_ptr<FolderBranch> b = std::make_unique<FolderBranch>(std::move(data));
            branch->AddChildFolder(std::move(b));
        }
    }
    branch->SetIsChildLoaded(true);
}


std::unique_ptr<FileData> LoadFileData(GFile *file, GFileInfo* info, bool issinglefile){
    std::unique_ptr<FileData> newfile = std::make_unique<FileData>();
    newfile->isVirtual = false;
    newfile->file = file;
    newfile->fileInfo = info;
    newfile->absoPath = g_file_get_path(file);
    newfile->fileName = g_file_get_basename(file);
    newfile->sortingCode1 = CalculateSortingCode(newfile->fileName);
    newfile->icon = g_file_query_info(file,
        "standard::icon",
        G_FILE_QUERY_INFO_NONE,
        nullptr,
        nullptr) ?
        g_file_info_get_icon(g_file_query_info(file,
            "standard::icon",
            G_FILE_QUERY_INFO_NONE,
            nullptr,
            nullptr)) :
        g_content_type_get_icon("text/plain");
    newfile->type = g_file_info_get_file_type(info);
    return newfile;
}

void ReadFileText(FileData *file, char **output){
    g_file_load_contents(file->file, nullptr, output, nullptr, nullptr, nullptr);
}

}// namespace filemanager
