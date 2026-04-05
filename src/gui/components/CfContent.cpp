#include "CfContent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <string>


CfContent::CfContent(){
    m_viewport = GTK_VIEWPORT(gtk_viewport_new(nullptr, nullptr));
    m_base = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    gtk_scrolled_window_set_child(m_base, GTK_WIDGET(m_viewport));
    gtk_viewport_set_scroll_to_focus(m_viewport, false);
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

GtkWidget *CfContent::getContentWidget(){
    return m_contentWidget;
}

void CfContent::setContentWidget(GtkWidget *widget){
    gtk_viewport_set_child(m_viewport, widget);
    m_contentWidget = widget;
}

std::string &CfContent::getContentName(){
    return m_contentName;
}

CfContent *CfContent::getParent(){
    return m_parent;
}

void CfContent::setParent(CfContent* parent){
    m_parent = parent;
}

CfContent *CfContent::getChild(){
    return m_child;
}

void CfContent::setChild(CfContent* child){
    m_child = child;
}

void CfContent::setContentName(const std::string &name){
    m_contentName = name;
    //call event callbacks
    for (void (*callback)(const std::string&, CfContent*) : m_nameChangedCallbacks) {
        callback(name, this);
    }
}

void CfContent::AddNameChangedCallback(void (*callback)(const std::string&, CfContent*)){
    m_nameChangedCallbacks.emplace(callback);
}

void CfContent::RemoveNameChangedCallback(void (*callback)(const std::string&, CfContent*)){
    m_nameChangedCallbacks.erase(callback);
}

//virtual
void CfContent::Destroy(){
    //call event callbacks
    delete this;
}

void CfContent::ParentDataChanged(CfContent *parent){
    //
}

void CfContent::ChildDataChanged(CfContent *child){
    //
}
