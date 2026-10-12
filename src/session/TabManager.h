#ifndef TABDATA_H_
#define TABDATA_H_

#include "layouts/tab/CfTabLayout.h"
#include <vector>

typedef struct AppContext AppContext ;

class TabManager {
public:
    TabManager(AppContext& context);

    CfTabLayout* GetFocusedTabLayout();
    void SetFocusedTabLayout(CfTabLayout* tablayout);

private:
    std::vector<CfTabLayout*> m_tabLayouts;
    CfTabLayout* m_focusedTabLayout = nullptr;
};

#endif
