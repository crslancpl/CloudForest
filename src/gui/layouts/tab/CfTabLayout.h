#ifndef CFTABLAYOUT_H_
#define CFTABLAYOUT_H_



#include "components/CfContent.h"
#include "CfTabSwitcher.h"

#include <memory>
#include <unordered_map>
#include <vector>

class CfTabLayout : public CfContent{
public:
    CfTabLayout();
    ~CfTabLayout();

    CfTabSwitcher& Add(std::unique_ptr<CfContent> content);
    void Remove(CfContent& content, CfTabSwitcher& switcher);

    void Show(CfContent& content);
    void SwitchTo(CfTabSwitcher& switcher);

private:
    std::unordered_map<CfContent*, CfTabSwitcher*> m_switcherMap;
    std::vector<std::unique_ptr<CfTabSwitcher>> m_childList;// switcher holds the ownership of content
    TabSwitcherTypes m_tabSwitcherType;
    GtkBox *m_base; // it is a GtkBox
    GtkStack *m_stack;
    GtkScrolledWindow *m_switcherScrolledWindow;
    GtkBox *m_switcherArea;
    CfTabSwitcher* m_activeSwitcher = nullptr;

    void ChildSwitcherClosed(CfTabSwitcher& switcher);
};

#endif
