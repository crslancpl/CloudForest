#include "CfTabLayout.h"

#include "CfTabSwitcher.h"
#include "CfTab_if.h"
#include "src/session/TabData.h"

#include <gtk/gtk.h>
#include <memory>
#include <utility>


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

void CfTabLayout::Add(std::unique_ptr<CfContent> content){
    //
    std::unique_ptr<CfTabSwitcher> newswitcher = std::make_unique<CfTabSwitcher>(std::move(content));
    newswitcher->SetParent(this);
    newswitcher->OnClose(&CfTabLayout::ChildSwitcherClosed);
    newswitcher->OnSwitch(&CfTabLayout::ChildSwitcherSwitched);
    gtk_box_append(m_switcherArea, GTK_WIDGET(newswitcher->GetBaseWidget()));
    gtk_stack_add_child(m_stack, newswitcher->GetContent()->GetBaseWidget());
    //content.SetParent(this);
    m_childList.emplace_back(std::move(newswitcher));
}

void CfTabLayout::Show(CfContent& content){
    GtkStackPage* page = gtk_stack_get_page(m_stack, content.GetBaseWidget());
    if (page == nullptr) {
        return;
    }

    gtk_stack_set_visible_child(m_stack, content.GetBaseWidget());
}

void CfTabLayout::Remove(CfContent& content, CfTabSwitcher& switcher){
    gtk_stack_remove(m_stack, content.GetBaseWidget());
    gtk_box_remove(m_switcherArea, switcher.GetBaseWidget());
    int itr = 0;

    for (const std::unique_ptr<CfTabSwitcher> &childswitcher : m_childList) {
        if (childswitcher.get() == &switcher) {
            m_childList.erase(m_childList.begin() + itr);
            return;
        }
        itr++;
    }
}

/*
 * Private
 */

void CfTabLayout::ChildSwitcherClosed(CfTabSwitcher* switcher){
    //
    this->Remove(*switcher->GetContent(), *switcher);
}

void CfTabLayout::ChildSwitcherSwitched(CfTabSwitcher* switcher){
    //
    this->Show(*switcher->GetContent());
}
