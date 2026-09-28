#include "FileReader.h"

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

std::unique_ptr<FileData> LoadFileData(GFile *file, GFileInfo* info, bool issinglefile){
    std::unique_ptr<FileData> newfile = std::make_unique<FileData>();
    newfile->isVirtual = false;
    newfile->file = file;
    newfile->fileInfo = info;
    newfile->absoPath = g_file_get_path(file);
    newfile->fileName = g_file_get_basename(file);
    newfile->sortingCode1 = CalculateSortingCode(newfile->fileName.c_str());
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

void ReadFileText(const FileData *file, char **output){
    g_file_load_contents(file->file, nullptr, output, nullptr, nullptr, nullptr);
}

}// namespace filemanager
