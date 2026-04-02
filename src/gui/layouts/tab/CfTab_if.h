#ifndef CFTAB_IF_H_
#define CFTAB_IF_H_

#include <gtk/gtk.h>

#include "CfTabLayout.h"
#include "../../components/CfContent.h"


namespace tablayout {
    CfTabLayout* NewTabLayout();
    void Show(CfContent *page);
    void SwitchTo(CfContent *page);
    void MovePage(CfContent *page, CfTabLayout *newtab);
}

#endif
