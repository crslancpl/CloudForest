#include "CfContent.h"
#include "toolset/event/Event.h"

#include <cstdio>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <string>


CfContent::CfContent(){
    m_viewport = GTK_VIEWPORT(gtk_viewport_new(nullptr, nullptr));
    m_base = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    gtk_scrolled_window_set_child(m_base, GTK_WIDGET(m_viewport));
    gtk_viewport_set_scroll_to_focus(m_viewport, false);

    m_eventMap = {
        {CFCONTENT_CLASS_NAME_CHANGED, SimpleEvent()}
    };
}

CfContent::~CfContent(){
    //
}

GtkWidget *CfContent::GetBaseWidget(){
    return GTK_WIDGET(m_base);
}

void CfContent::SetHorizontalExpand(bool expand){
    gtk_widget_set_hexpand(GTK_WIDGET(m_base), expand);
}

void CfContent::SetVerticalExpand(bool expand){
    gtk_widget_set_vexpand(GTK_WIDGET(m_base), expand);
}

void CfContent::SetDefaultSize(int width, int height){
    gtk_widget_set_size_request(GTK_WIDGET(m_base), width, height);
}

GtkWidget *CfContent::GetContentWidget(){
    return m_contentWidget;
}

void CfContent::SetContentWidget(GtkWidget *widget){
    gtk_viewport_set_child(m_viewport, widget);
    m_contentWidget = widget;
}

std::string &CfContent::GetContentName(){
    return m_contentName;
}

CfContent *CfContent::GetParent(){
    return m_parent;
}

void CfContent::SetParent(CfContent* parent){
    m_parent = parent;
}

CfContent *CfContent::GetChild(){
    return m_child;
}

void CfContent::SetChild(CfContent* child){
    m_child = child;
}

void CfContent::SetContentName(const std::string &name){
    m_contentName = name;
    //call event callbacks
    const SimpleEvent &event = m_eventMap.at(CFCONTENT_CLASS_NAME_CHANGED);
    for (EventCallback callback : event.GetCallbackSet()) {
        ((NameChangedCallback)callback)(name, this);
    }
}

void CfContent::Listen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if (itr != m_eventMap.end()) {
        itr->second.Connect(callback);
    }
}

void CfContent::StopListen(Signal signal, EventCallback callback){
    auto itr = m_eventMap.find(signal);
    if (itr != m_eventMap.end()) {
        itr->second.Disconnect(callback);
    }
}


//virtual

void CfContent::Destroy(){
    delete this;
}

void CfContent::ParentDataChanged(CfContent *parent){
    //
}

void CfContent::ChildDataChanged(CfContent *child){
    //
}



CfContent* cfcontent::PackAsCfContent(GtkWidget *widget){
    auto content = new CfContent();
    content->SetContentWidget(widget);
    return content;
}
