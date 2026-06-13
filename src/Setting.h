#ifndef SETTING_H_
#define SETTING_H_

#include "datatypes/extension.h"
#include "toolset/event/Event.h"

#include <unordered_set>

namespace setting{

enum Signal{
    SETTING_EXTENSION_ENABLED,
    SETTING_EXTENSION_DISABLED
};

void Listen(Signal signal, EventCallback callback);
void StopListen(Signal signal, EventCallback callback);

std::unordered_set<Extension*> &GetAllExtensions();
void AddExtension(Extension* extension);
void RemoveExtension(Extension* extension);
void EnableExtension(Extension* extension);
void DisableExtension(Extension* extension);

}
#endif
