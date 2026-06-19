#ifndef TABDATA_H_
#define TABDATA_H_

#include "layouts/tab/CfTabLayout.h"

namespace session {

CfTabLayout* GetFocusedTabLayout();
void SetFocusedTabLayout(CfTabLayout* tablayout);

}// namespace session

#endif
