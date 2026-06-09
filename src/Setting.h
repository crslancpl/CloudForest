#ifndef SETTING_H_
#define SETTING_H_

#include "datatypes/common.h"
#include "datatypes/extension.h"

#include <unordered_set>

namespace setting{

enum Event{
    SETTING_EXTENSION_ENABLED,
    SETTING_EXTENSION_DISABLED
};

void ListenEvent(Event event, EventCallback callback);

std::unordered_set<Extension*> &GetAllExtensions();
void AddExtension(Extension* extension);
void RemoveExtension(Extension* extension);
void EnableExtension(Extension* extension);
void DisableExtension(Extension* extension);

}
#endif
