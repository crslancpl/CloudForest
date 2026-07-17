#ifndef CFTABSWITCHER_H_
#define CFTABSWITCHER_H_

#include <gtk/gtk.h>
#include <memory>

#include "components/CfContent.h"

class CfContent;
class CfTabLayout;
class CfTabSwitcher;

typedef void (CfTabLayout::*TabSwitchedCallback)(CfTabSwitcher*);
typedef void (CfTabLayout::*TabClosedCallback)(CfTabSwitcher*);

enum class TabSwitcherTypes:char{
    NONE,
    HORIZONTAL_TOP,
    HORIZONTAL_BOTTOM,
    VERTICAL_LEFT,
    VERTICAL_RIGHT
};

class CfTabSwitcher:public CfContent{
public:
    CfTabSwitcher(std::unique_ptr<CfContent> content);
    ~CfTabSwitcher();

    void ChildDataChanged(CfContent* child) override;
    GtkWidget* GetBaseWidget() override;

    CfContent* GetContent();

    void SetParent(CfTabLayout* parent);
    void SetText(const char *text);
    void Switch();
    void Close();

    /*
     * ONLY PARENT TAB LAYOUT SHOULD CONNECT THE EVENTS
     */
    void OnClose(TabClosedCallback callback);
    void OnSwitch(TabSwitchedCallback callback);

private:
    GtkBox *m_baseBox;
    GtkButton *m_switchButton;
    GtkButton *m_closeButton;
    GtkWidget *m_page;

    CfTabLayout* m_parent;
    std::unique_ptr<CfContent> m_content;

    TabClosedCallback m_closedCallback;
    TabSwitchedCallback m_switchedCallback;
};

#endif
