#include "ExtensionPage.h"
#include <gtk/gtk.h>

ExtensionPage::ExtensionPage(){
    this->AddHeader1("Extension");
    this->AddHeader2("Enabled");
    m_enabledExtensonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    this->AddWidget(GTK_WIDGET(m_enabledExtensonBox));
    this->AddHeader2("Disabled");
    m_disabledExtensonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    this->AddWidget(GTK_WIDGET(m_disabledExtensonBox));
}
