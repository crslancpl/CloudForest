#ifndef CFCONTENT_H_
#define CFCONTENT_H_

#include "CfComponent.h"
#include "toolset/event/Event.h"

#include <gtk/gtk.h>
#include <string>
#include <unordered_map>

//forward declaration
class CfContent;

typedef void(*NameChangedCallback)(const std::string&, CfContent*);

class CfContent : public CfComponent{
public:
    CfContent();

    GtkWidget *GetBaseWidget() override;
    void SetHorizontalExpand(bool expand);
    void SetVerticalExpand(bool expand);
    void SetDefaultSize(int width, int height);

    GtkWidget *GetContentWidget();
    void SetContentWidget(GtkWidget* widget);
    std::string &GetContentName();
    void SetContentName(const std::string &name);
    CfContent *GetParent();
    void SetParent(CfContent* parent);
    CfContent *GetChild();
    void SetChild(CfContent* child);

    enum Signal{
        CFCONTENT_CLASS_NAME_CHANGED
    };

    void Listen(Signal signal, EventCallback callback);
    void StopListen(Signal signal, EventCallback callback);

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

    std::unordered_map<Signal, SimpleEvent> m_eventMap;
};

namespace cfcontent{
CfContent *PackAsCfContent(GtkWidget *widget);
}

#endif
