#ifndef SETTING_H_
#define SETTING_H_

#include "datatypes/common.h"

#include <unordered_set>

namespace setting{

std::unordered_set<Extension*> &GetAllExtensions();
void AddExtension(const char* name, const char* folderPath, const char* description, bool enabled);
void RemoveExtension(Extension* extension);
void EnableExtension(Extension* extension);
void DisableExtension(Extension* extension);

}
#endif
