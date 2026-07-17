#include "CfTabSwitcher.h"

#include "CfTabLayout.h"
#include "components/CfContent.h"

#include <gtk/gtk.h>
#include <memory>



static void SwitchButtonClicked(GtkButton *self, CfTabSwitcher *switchbutton){
    switchbutton->Switch();
}
static void CloseButtonClicked(GtkButton *self, CfTabSwitcher *switchbutton){
    switchbutton->Close();
}

CfTabSwitcher::CfTabSwitcher(std::unique_ptr<CfContent> content):
    m_content(std::move(content))
    {
    m_closeButton = GTK_BUTTON(gtk_button_new());
    m_switchButton = GTK_BUTTON(gtk_button_new_with_label(m_content->GetContentName().c_str()));
    m_baseBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    m_content->SetParent(this);

    gtk_button_set_icon_name(m_closeButton, "window-close-symbolic");

    gtk_widget_set_size_request(GTK_WIDGET(m_switchButton), 50, 15);
    gtk_widget_set_size_request(GTK_WIDGET(m_closeButton), 15, 15);

    gtk_widget_add_css_class(GTK_WIDGET(m_baseBox), "switcher-base-box");

    gtk_box_append(m_baseBox, GTK_WIDGET(m_switchButton));
    gtk_box_append(m_baseBox, GTK_WIDGET(m_closeButton));

    g_signal_connect(m_switchButton, "clicked", G_CALLBACK(SwitchButtonClicked), this);
    g_signal_connect(m_closeButton, "clicked", G_CALLBACK(CloseButtonClicked), this);
}

CfTabSwitcher::~CfTabSwitcher(){
    //
}

void CfTabSwitcher::ChildDataChanged(CfContent* child){
    this->SetText(child->GetContentName().c_str());
}

GtkWidget* CfTabSwitcher::GetBaseWidget(){
    return GTK_WIDGET(m_baseBox);
}

CfContent* CfTabSwitcher::GetContent(){
    return m_content.get();
}

void CfTabSwitcher::SetParent(CfTabLayout* parent){
    m_parent = parent;
}

void CfTabSwitcher::SetText(const char *text){
    gtk_button_set_label(m_switchButton, text);
}

void CfTabSwitcher::Switch(){
    (m_parent->*m_switchedCallback)(this);
}

void CfTabSwitcher::Close(){
    (m_parent->*m_closedCallback)(this);
}

void CfTabSwitcher::OnClose(TabClosedCallback callback){
    m_closedCallback = callback;
}

void CfTabSwitcher::OnSwitch(TabSwitchedCallback callback){
    m_switchedCallback = callback;
}
