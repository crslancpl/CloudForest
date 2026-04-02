#ifndef CFTABSWITCHER_H_
#define CFTABSWITCHER_H_

#include <gtk/gtk.h>

#include "../../components/CfComponent.h"

class CfContent;
class CfTabLayout;

enum class TabSwitcherTypes:char{
    NONE,
    HORIZONTAL_TOP,
    HORIZONTAL_BOTTOM,
    VERTICAL_LEFT,
    VERTICAL_RIGHT
};

class CfTabSwitcher:public CfComponent{
public:
    CfTabSwitcher(CfContent *content, CfTabLayout *parent);
    GtkWidget * GetBaseWidget() override;
    void SetText(const char *text);
    void Switch();
    void Close();
private:
    GtkBox *m_baseBox;
    GtkButton *m_switchButton;
    GtkButton *m_closeButton;
    GtkWidget *m_page;
    CfTabLayout *m_parentTabLayout;
    CfContent *m_content;
};

#endif
