#ifndef EXTENSIONPAGE_H_
#define EXTENSIONPAGE_H_

#include "SettingPage.h"

class ExtensionPage : public SettingPage {
public:
    ExtensionPage();

    void ShowExtensions();
private:
    GtkBox* m_enabledExtensonBox;
    GtkBox* m_disabledExtensonBox;
};

#endif
