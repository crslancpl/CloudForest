#ifndef CFCONTENT_H_
#define CFCONTENT_H_

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <string>
#include <unordered_set>

#include "CfComponent.h"

class CfContent : public CfComponent{
public:
    CfContent();

    GtkWidget *GetBaseWidget() override;
    void SetHorizontalExpand(bool expand);
    void SetVerticalExpand(bool expand);
    void SetDefaultSize(int width, int height);

    GtkWidget *getContentWidget();
    void setContentWidget(GtkWidget* widget);
    std::string &getContentName();
    void setContentName(const std::string &name);
    CfContent *getParent();
    void setParent(CfContent* parent);
    CfContent *getChild();
    void setChild(CfContent* child);

    void AddNameChangedCallback(void (*callback)(const std::string&, CfContent*));
    void RemoveNameChangedCallback(void (*callback)(const std::string&, CfContent*));

    virtual void Destroy();
    virtual void ChildDataChanged(CfContent* child);
    virtual void ParentDataChanged(CfContent* parent);
    //void SetIcon()

protected:
    CfContent *m_child = nullptr;
    CfContent *m_parent = nullptr;

private:
    GtkScrolledWindow *m_base;
    GtkViewport *m_viewport;
    GtkWidget *m_contentWidget;
    std::string m_contentName;
    std::unordered_set<void(*)(const std::string&, CfContent*)> m_nameChangedCallbacks;
};

namespace cfcontent{
    CfContent *PackAsCFContent(GtkWidget *widget);
}

#endif
