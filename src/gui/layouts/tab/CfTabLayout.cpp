#include "CfTabLayout.h"

#include "CfTabSwitcher.h"
#include "CfTab_if.h"
#include "src/session/TabData.h"

#include <gtk/gtk.h>


CfTabLayout::CfTabLayout(){
    m_base = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    m_stack = GTK_STACK(gtk_stack_new());
    m_switcherArea = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    m_switcherScrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());

    gtk_widget_set_hexpand(GTK_WIDGET(m_switcherScrolledWindow), true);
    gtk_scrolled_window_set_child(m_switcherScrolledWindow, GTK_WIDGET(m_switcherArea));
    gtk_box_append(GTK_BOX(m_base), GTK_WIDGET(m_switcherScrolledWindow));
    gtk_box_append(GTK_BOX(m_base), GTK_WIDGET(m_stack));
    SetContentWidget(GTK_WIDGET(m_base));
    session::SetFocusedTabLayout(this);
}


void CfTabLayout::Show(CfContent& content){
    GtkStackPage* page = gtk_stack_get_page(m_stack, content.GetBaseWidget());
    if(page == nullptr){
        auto switcher = new CfTabSwitcher(content, *this);
        m_switcherMap.insert({&content, switcher});
        switcher->SetText(content.GetContentName().c_str());
        gtk_box_append(m_switcherArea, GTK_WIDGET(switcher->GetBaseWidget()));
        gtk_stack_add_child(m_stack, content.GetBaseWidget());
        content.SetParent(this);
    }

    gtk_stack_set_visible_child(m_stack, content.GetBaseWidget());
}

void CfTabLayout::Remove(CfContent& content, CfTabSwitcher& switcher){
    gtk_stack_remove(m_stack, content.GetBaseWidget());
    auto itr = m_switcherMap.find(&content);
    if (itr == m_switcherMap.end()) {
        return;
    }
    gtk_box_remove(m_switcherArea, switcher.GetBaseWidget());
}

void CfTabLayout::ChildDataChanged(CfContent* child){
    auto itr = m_switcherMap.find(child);

    if(itr != m_switcherMap.end()){
        itr->second->SetText(child->GetContentName().c_str());
    }
}
