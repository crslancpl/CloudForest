#include "Setting.h"
#include "datatypes/extension.h"

#include <unordered_set>

typedef void (*ExtensionEnabledCallback)(Extension*);
typedef void (*ExtensionDisabledCallback)(Extension*);

static std::unordered_set<Extension*> extension_list;

static std::unordered_set<ExtensionEnabledCallback> extension_enabled_callback_list;
static std::unordered_set<ExtensionDisabledCallback> extension_disabled_callback_list;

namespace setting{


void ListenEvent(Event event, EventCallback callback){
    switch (event) {
    case SETTING_EXTENSION_ENABLED:
        extension_enabled_callback_list.insert((ExtensionEnabledCallback)callback);
        break;
    case SETTING_EXTENSION_DISABLED:
        extension_disabled_callback_list.insert((ExtensionDisabledCallback)callback);
        break;
    default:
        break;
    }
}

std::unordered_set<Extension*> &GetAllExtensions(){
    return extension_list;
}

void AddExtension(Extension* extension){
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
    for ( ExtensionEnabledCallback callback : extension_enabled_callback_list) {
        callback(extension);
    }
}

void DisableExtension(Extension* extension){
    extension->enabled = false;
    for ( ExtensionDisabledCallback callback : extension_disabled_callback_list) {
        callback(extension);
    }
}

}// namespace setting
