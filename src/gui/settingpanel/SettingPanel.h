#ifndef SETTINGPANEL_H_
#define SETTINGPANEL_H_

#include "SettingPage.h"
#include "headerbar/Headerbar.h"
#include "src/gui/windows/MainWindow.h"
#include "src/gui/windows/Window.h"

#include <gtk/gtk.h>
#include <gtk/gtkdropdown.h>
#include <gtk/gtkshortcut.h>

//forward declaration
class CfLayout;
typedef struct AppUI AppUI ;

class SettingPanel : public Window{
public:
    SettingPanel(AppUI& appui);

    void Show() override;
    void SwitchPage(const char* name);
    void AddTabButton(const char* name);
    void AddPage(const char* name, SettingPage* page);

private:
    void BindUI(GtkBuilder* builder);

    AppUI& m_appUI;

    CfLayout *m_baseLayout;// Saperates TabButtonBox and Stack
    GtkBox *m_tabButtonBox;// left-hand side
    GtkGrid *m_tabButtonBoxSeparator;
    GtkButton *m_closeButton;
    GtkStack *m_stack;// right-hand side
};

#endif
