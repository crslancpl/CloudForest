#ifndef EXTENSIONMANAGER_H_
#define EXTENSIONMANAGER_H_

#include "datatypes/extension.h"
#include "toolset/event/Event.h"

#include <memory.h>
#include <memory>
#include <vector>

class ExtensionManager{
public:
    Event<Extension*> EExtensionEnabled;
    Event<Extension*> EExtensionDisabled;

    const std::vector<std::unique_ptr<Extension>> &GetAllExtensions();
    void AddExtension(std::unique_ptr<Extension> extension);

    void RemoveExtension(Extension* extension);
    void EnableExtension(Extension* extension);
    void DisableExtension(Extension* extension);
private:
    std::vector<std::unique_ptr<Extension>> extensions;
};

#endif
