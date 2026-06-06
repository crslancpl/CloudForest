#ifndef SETTINGPANEL_H_
#define SETTINGPANEL_H_

#include "SettingPage.h"
#include "src/gui/windows/MainWindow.h"
#include "src/gui/windows/Window.h"

#include <gtk/gtk.h>
#include <gtk/gtkdropdown.h>
#include <gtk/gtkshortcut.h>

//forward declaration
class CfLayout;

class SettingPanel : public Window{
public:
    SettingPanel(MainWindow* parentwindow);

    void Show() override;
    void SwitchPage(const char* name);
    void AddTabButton(const char* name);
    void AddPage(const char* name, SettingPage* page);

private:
    void BindTabButtons(GtkBuilder* builder);

    MainWindow *m_parentWindow;// the main window will only be freed on app closed

    CfLayout *m_baseLayout;// Saperates TabButtonBox and Stack
    GtkBox *m_tabButtonBox;// left-hand side
    GtkGrid *m_tabButtonBoxSeparator;
    GtkButton *m_closeButton;
    GtkStack *m_stack;// right-hand side
};

#endif
