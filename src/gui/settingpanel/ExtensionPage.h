#ifndef EXTENSIONPAGE_H_
#define EXTENSIONPAGE_H_

#include "SettingPage.h"

class ExtensionPage : public SettingPage {
public:
    ExtensionPage();

    void AddEnabledExtension();
private:
    GtkBox* m_enabledExtensonBox;
    GtkBox* m_disabledExtensonBox;
};

#endif
