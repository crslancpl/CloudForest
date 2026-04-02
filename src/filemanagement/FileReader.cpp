#include "FileManagement_if.h"
#include "src/filemanagement/FileManagement_if.h"

#include "datatypes/file_data.h"

void filemanagement::EnumerateFolderChild(GFile* folder, void (*callback)(GFile*, GFileInfo*)){
    /*
     * Enumerate children in folder and call the callback function
     * every child file/folder. Callback nullptr and nullptr when the
     * children are all enumerated.
     */

    GFileEnumerator *enumerator = g_file_enumerate_children(folder, "*", G_FILE_QUERY_INFO_NONE,
        nullptr, nullptr);

    GFileInfo *info;
    while ((info = g_file_enumerator_next_file(enumerator, nullptr, nullptr)) != nullptr) {
        GFile *child = g_file_enumerator_get_child(enumerator, info);
        if (child == nullptr|| info == nullptr) continue;
        callback(child, info);
    }

    callback(nullptr, nullptr);
}

datatypes::FileData* filemanagement::LoadFileData(GFile *file){
    datatypes::FileData *result = new datatypes::FileData();
    result->file = file;
    result->icon = g_file_query_info(file,
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
    result->fileAbsoPath = g_file_get_path(file);
    result->fileName = g_file_get_basename(file);
    return result;
}

void filemanagement::ReadFileText(GFile *file, char **output){
    g_file_load_contents(file,nullptr, output, nullptr, nullptr,nullptr);
}
