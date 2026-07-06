#include "CfTab_if.h"

#include "CfTabLayout.h"
#include "components/CfContent.h"
#include "layouts/tab/CfTabSwitcher.h"

#include <unordered_map>

static std::unordered_map<CfContent*, CfTabSwitcher*> content_switcher_map;


CfTabSwitcher* tablayout::GetSwitcher(CfContent& content){
    auto result = content_switcher_map.find(&content);
    return result!=content_switcher_map.end() ? result->second : nullptr;
}
