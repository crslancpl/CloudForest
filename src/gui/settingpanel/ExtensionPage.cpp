#include "ExtensionPage.h"

#include "datatypes/common.h"
#include "src/Setting.h"

#include <cstdio>
#include <gtk/gtk.h>
#include <unordered_set>

ExtensionPage::ExtensionPage(){
    this->AddHeader1("Extension");
    this->AddHeader2("Enabled");
    m_enabledExtensonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    this->AddWidget(GTK_WIDGET(m_enabledExtensonBox));
    this->AddHeader2("Disabled");
    m_disabledExtensonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    this->AddWidget(GTK_WIDGET(m_disabledExtensonBox));
}

void ExtensionPage::ShowExtensions(){
    //
    std::unordered_set<Extension*> &allExtensions = setting::GetAllExtensions();
    for(auto extension : allExtensions){
        printf("%s\n", extension->name);
    }
}
