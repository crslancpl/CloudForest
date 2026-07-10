#ifndef SETTING_H_
#define SETTING_H_

#include "datatypes/extension.h"
#include "toolset/event/Event.h"

#include <memory.h>
#include <memory>
#include <vector>

namespace setting{

enum Signal{
    SETTING_EXTENSION_ENABLED,
    SETTING_EXTENSION_DISABLED
};

void Listen(Signal signal, EventCallback callback);
void StopListen(Signal signal, EventCallback callback);

const std::vector<std::unique_ptr<Extension>> &GetAllExtensions();
void AddExtension(std::unique_ptr<Extension> extension);
void RemoveExtension(Extension* extension);
void EnableExtension(Extension* extension);
void DisableExtension(Extension* extension);

}
#endif
