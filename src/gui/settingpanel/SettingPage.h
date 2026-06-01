#ifndef SETTINGPAGE_H_
#define SETTINGPAGE_H_

#include "src/gui/components/CfContent.h"

#include <gtk/gtk.h>

class SettingPage : public CfContent{
public:
    SettingPage();

    GtkLabel* AddHeader1(const char* text);
    GtkLabel* AddHeader2(const char* text);
    void AddWidget(GtkWidget* child);
protected:
    GtkBox* m_baseBox;
};

#endif
