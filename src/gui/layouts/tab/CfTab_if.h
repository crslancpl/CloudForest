#ifndef CFTAB_IF_H_
#define CFTAB_IF_H_

#include <gtk/gtk.h>

#include "../../components/CfContent.h"

class CfTabLayout;
class CfTabSwitcher;

namespace tablayout {
    CfTabLayout* NewTabLayout();
    void Show(CfContent *content);
    void SwitchTo(CfContent *content);
    void MovePage(CfContent *content, CfTabLayout *newtab);
    CfTabSwitcher* GetSwitcher(CfContent *content);
    void AddContentSwitcherPair(CfContent *content, CfTabSwitcher *switcher);
}

#endif
