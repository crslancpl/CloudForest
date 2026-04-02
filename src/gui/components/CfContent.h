#ifndef CFCONTENT_H_
#define CFCONTENT_H_

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <string>

#include "CfComponent.h"

class CfContent : public CfComponent{
public:
    CfContent();

    GtkWidget *GetBaseWidget() override;

    void SetHorizontalExpand(bool expand);
    void SetVerticalExpand(bool expand);
    void SetDefaultSize(int width, int height);
    void SetContentWidget(GtkWidget *ui);
    GtkWidget *GetContentWidget();
    std::string &GetContentName();
    void SetContentName(const std::string &name);
    virtual void Destroy();
    //void SetIcon()
private:
    GtkScrolledWindow *m_base;
    GtkViewport *m_viewport;
    GtkWidget *m_child;
    std::string m_contentName;
};

namespace cfcontent{
    CfContent *PackAsCFContent(GtkWidget *widget);
}

#endif
