#include "FileReader.h"

#include "datatypes/file.h"

#include <gio/gio.h>

/*
 * File Tree:
 * To reduce resource usage, file tree would not read its chile/descendant automatically.
 * Use ExpandFileBranch() to read the child of a branch.
 */

namespace filemanagement {

FileBranch* CreateFileTree(FileData* rootfolderdata){
    if(rootfolderdata->type != G_FILE_TYPE_DIRECTORY){
        return nullptr;
    }

    FileBranch* fb = new FileBranch();
    fb->name = rootfolderdata->fileName;
    fb->fileData = rootfolderdata;
    fb->parentBranch = nullptr;
    return fb;
}

void ExpandFileBranch(FileBranch *branch){
    if(!branch->childBranch.empty()){
        // already expanded
        return;
    }


    GFileEnumerator *enumerator = g_file_enumerate_children(branch->fileData->file, "*", G_FILE_QUERY_INFO_NONE,
        nullptr, nullptr);

    GFileInfo* info;

    while ((info = g_file_enumerator_next_file(enumerator, nullptr, nullptr)) != nullptr) {
        GFile *child = g_file_enumerator_get_child(enumerator, info);
        FileData* data = LoadFileData(child, info);
        if (child == nullptr|| data->fileInfo == nullptr) continue;

        FileBranch* childbranch = new FileBranch();
        childbranch->fileData = data;
        childbranch->parentBranch = branch;
        branch->childBranch.emplace(childbranch);
    }
}


FileData* LoadFileData(GFile *file, GFileInfo* info){
    FileData *newfile = new FileData();
    newfile->isVirtual = false;
    newfile->file = file;
    newfile->fileInfo = info;
    newfile->absoPath = g_file_get_path(file);
    newfile->fileName = g_file_get_basename(file);
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

}// namespace filemanagment
