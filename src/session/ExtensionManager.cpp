#include "ExtensionManager.h"
#include "datatypes/extension.h"
#include "toolset/event/Event.h"

#include <memory>
#include <vector>


const std::vector<std::unique_ptr<Extension>> &ExtensionManager::GetAllExtensions(){
    return extensions;
}

void ExtensionManager::AddExtension(std::unique_ptr<Extension> extension){
    extensions.emplace_back(std::move(extension));
}

void ExtensionManager::RemoveExtension(Extension* extension){
    //Removing the extension(Delete the folder of extension)
    int itr = 0;
    for (const std::unique_ptr<Extension>& ext : extensions) {
        if (ext.get() == extension) {
            extensions.erase(extensions.begin() + itr);
            break;
        }
        itr ++;
    }

    delete [] extension->name;
    delete [] extension->folder;
    delete [] extension->description;
    delete extension;
}

void ExtensionManager::EnableExtension(Extension* extension){
    extension->enabled = true;
    EExtensionEnabled.Invoke(extension);
}

void ExtensionManager::DisableExtension(Extension* extension){
    extension->enabled = false;
    EExtensionDisabled.Invoke(extension);
}
