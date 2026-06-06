#include "Setting.h"
#include "datatypes/common.h"

#include <string.h>

static std::unordered_set<Extension*> extension_list;

namespace setting{

std::unordered_set<Extension*> &GetAllExtensions(){
    return extension_list;
}

void AddExtension(const char* name, const char* folderPath, const char* description, bool enabled){
    Extension* extension = new Extension();// freed on "RemoveExtension()" or app closed
    extension->name = strdup(name);
    extension->folder = strdup(folderPath);
    extension->description = strdup(description);
    extension->enabled = enabled;
    extension_list.insert(extension);
}

void RemoveExtension(Extension* extension){
    //Removing the extension(Delete the folder of extension)
    extension_list.erase(extension);
    delete [] extension->name;
    delete [] extension->folder;
    delete [] extension->description;
    delete extension;
}

void EnableExtension(Extension* extension){
    extension->enabled = true;
}
void DisableExtension(Extension* extension){
    extension->enabled = false;
}

}// namespace setting
