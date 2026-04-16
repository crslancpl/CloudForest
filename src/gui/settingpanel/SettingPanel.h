#ifndef SETTINGPANEL_H_
#define SETTINGPANEL_H_

#include <gtk/gtk.h>
#include <gtk/gtkdropdown.h>
#include <gtk/gtkshortcut.h>

//forward declaration
class CfLayout;

class SettingPanel{
public:
    SettingPanel();

    void Show();
    void SwitchPage();
    void AddTabButton(const char* name);
    GtkWindow* GetWindowWidget();


private:
    GtkWindow *m_window;
    CfLayout *m_baseLayout;// Saperates TabButtonBox and Stack
    GtkBox *m_tabButtonBox;// left-hand side
    GtkGrid *m_tabButtonBoxSeparator;
    GtkButton *m_closeButton;
    GtkStack *m_stack;// right-hand side

    void BindTabButtons(GtkBuilder* builder);
};

#endif
