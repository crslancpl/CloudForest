#include "TabData.h"
#include "layouts/tab/CfTabLayout.h"

CfTabLayout* focused_tab_layout;

namespace session {

CfTabLayout* GetFocusedTabLayout(){
    return focused_tab_layout;
}

void SetFocusedTabLayout(CfTabLayout* tablayout){
    focused_tab_layout = tablayout;
}


}// namespace session
