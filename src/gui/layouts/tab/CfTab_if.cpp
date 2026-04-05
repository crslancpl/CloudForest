#include "CfTab_if.h"
#include "CfTabLayout.h"
#include "src/gui/components/CfContent.h"
#include "src/gui/layouts/tab/CfTabSwitcher.h"

#include <unordered_map>

static CfTabLayout* focused_tablayout;
static std::unordered_map<CfContent*, CfTabSwitcher*> content_switcher_map;

CfTabLayout* tablayout::NewTabLayout(){
    CfTabLayout *newtablayout = new CfTabLayout();
    focused_tablayout = newtablayout;
    return focused_tablayout;
}

void tablayout::Show(CfContent *content){
    focused_tablayout->Show(content);
}

CfTabSwitcher* tablayout::GetSwitcher(CfContent *content){
    auto result = content_switcher_map.find(content);
    return result!=content_switcher_map.end() ? result->second : nullptr;
}

void tablayout::AddContentSwitcherPair(CfContent* content, CfTabSwitcher* switcher){
    content_switcher_map.emplace(content, switcher);
}
