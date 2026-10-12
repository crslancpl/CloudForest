#include "TabManager.h"

#include "layouts/tab/CfTabLayout.h"
#include "src/AppContext.h"


TabManager::TabManager(AppContext& context){}

CfTabLayout* TabManager::GetFocusedTabLayout(){
    return m_focusedTabLayout;
}

void TabManager::SetFocusedTabLayout(CfTabLayout* tablayout){
    m_focusedTabLayout = tablayout;
}
