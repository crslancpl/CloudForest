#ifndef CFTABLAYOUT_H_
#define CFTABLAYOUT_H_


#include <unordered_map>
#include "src/gui/components/CfContent.h"
#include "CfTabSwitcher.h"

class CfTabLayout : public CfContent{
public:
    CfTabLayout();

    void ChangeLayout(TabSwitcherTypes layouttype);
    void Show(CfContent *content);
    void Remove(CfContent *content);
    void ShowBlank();
private:
    std::unordered_map<CfContent*, CfTabSwitcher*> m_switcherMap;
    TabSwitcherTypes m_tabSwitcherType;
    GtkBox *m_base; // it is a GtkBox
    GtkStack *m_stack;
    GtkScrolledWindow *m_switcherScrolledWindow;
    GtkBox *m_switcherArea;
};

#endif
