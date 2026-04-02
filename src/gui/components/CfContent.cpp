#include "CfContent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>


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

void CfContent::SetContentWidget(GtkWidget *child){
    gtk_viewport_set_child(m_viewport, child);
    m_child = child;
}

GtkWidget *CfContent::GetContentWidget(){
    return m_child;
}

std::string &CfContent::GetContentName(){
    return m_contentName;
}

void CfContent::SetContentName(const std::string &name){
    m_contentName = name;
    //call event callbacks
}

void CfContent::Destroy(){
    //call event callbacks
    delete this;
}
