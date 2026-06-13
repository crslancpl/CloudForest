#include "Setting.h"
#include "datatypes/extension.h"
#include "toolset/event/Event.h"

#include <unordered_map>
#include <unordered_set>

typedef void (*ExtensionEnabledCallback)(Extension*);
typedef void (*ExtensionDisabledCallback)(Extension*);

static std::unordered_set<Extension*> extension_list;

//static std::unordered_set<ExtensionEnabledCallback> extension_enabled_callback_list;
//static std::unordered_set<ExtensionDisabledCallback> extension_disabled_callback_list;

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
