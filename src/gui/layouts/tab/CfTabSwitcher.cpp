#include "CfTabSwitcher.h"

#include "CfTabLayout.h"
#include "src/gui/components/CfContent.h"



static void SwitchButtonClicked(GtkButton *self, CfTabSwitcher *switchbutton){
    switchbutton->Switch();
}
static void CloseButtonClicked(GtkButton *self, CfTabSwitcher *switchbutton){
    switchbutton->Close();
}

CfTabSwitcher::CfTabSwitcher(CfContent *content,CfTabLayout *parent){
    m_content = content;

    m_closeButton = GTK_BUTTON(gtk_button_new());
    m_switchButton = GTK_BUTTON(gtk_button_new());
    m_baseBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    m_parentTabLayout = parent;

    gtk_button_set_icon_name(m_closeButton, "window-close-symbolic");

    gtk_widget_set_size_request(GTK_WIDGET(m_switchButton), 50, 15);
    gtk_widget_set_size_request(GTK_WIDGET(m_closeButton), 15, 15);

    gtk_widget_add_css_class(GTK_WIDGET(m_baseBox), "SwitcherBaseBox");

    gtk_box_append(m_baseBox, GTK_WIDGET(m_switchButton));
    gtk_box_append(m_baseBox, GTK_WIDGET(m_closeButton));

    g_signal_connect(m_switchButton, "clicked", G_CALLBACK(SwitchButtonClicked), this);
    g_signal_connect(m_closeButton, "clicked", G_CALLBACK(CloseButtonClicked), this);
}

GtkWidget* CfTabSwitcher::GetBaseWidget(){
    return GTK_WIDGET(m_baseBox);
}

void CfTabSwitcher::SetText(const char *text){
    gtk_button_set_label(m_switchButton, text);
}

void CfTabSwitcher::Switch(){
    m_parentTabLayout->Show(m_content);
}

void CfTabSwitcher::Close(){
    m_parentTabLayout->Remove(m_content);
    m_content->Destroy();
    delete this;
}
