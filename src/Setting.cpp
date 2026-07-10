#include "Setting.h"
#include "datatypes/extension.h"
#include "toolset/event/Event.h"

#include <memory>
#include <unordered_map>
#include <vector>

typedef void (*ExtensionEnabledCallback)(Extension*);
typedef void (*ExtensionDisabledCallback)(Extension*);

static std::vector<std::unique_ptr<Extension>> extension_list;

namespace setting{

static std::unordered_map<Signal, SimpleEvent> event_map = {
    {SETTING_EXTENSION_DISABLED, SimpleEvent()},
    {SETTING_EXTENSION_ENABLED, SimpleEvent()}
};

void Listen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void StopListen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

const std::vector<std::unique_ptr<Extension>> &GetAllExtensions(){
    return extension_list;
}

void AddExtension(std::unique_ptr<Extension> extension){
    extension_list.emplace_back(std::move(extension));
}

void RemoveExtension(Extension* extension){
    //Removing the extension(Delete the folder of extension)
    int itr = 0;
    for (const std::unique_ptr<Extension>& ext : extension_list) {
        if (ext.get() == extension) {
            extension_list.erase(extension_list.begin() + itr);
            break;
        }
        itr ++;
    }

    delete [] extension->name;
    delete [] extension->folder;
    delete [] extension->description;
    delete extension;
}

void EnableExtension(Extension* extension){
    extension->enabled = true;
    const SimpleEvent &event = event_map.at(SETTING_EXTENSION_ENABLED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((ExtensionEnabledCallback)callback)(extension);
    }
}

void DisableExtension(Extension* extension){
    extension->enabled = false;
    const SimpleEvent &event = event_map.at(SETTING_EXTENSION_DISABLED);
    for(EventCallback callback : event.GetCallbackSet()){
        ((ExtensionDisabledCallback)callback)(extension);
    }
}

}// namespace setting
