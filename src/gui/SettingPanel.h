#ifndef SETTINGPANEL_H_
#define SETTINGPANEL_H_

#include <gtk/gtk.h>

class SettingPanel{
public:
    void Init();
    void Show();

    GtkWindow *Window;
    GtkBox *BaseBox;// Saperates TabButtonBox and Stack

    GtkBox *TabButtonBox;// left-hand side
    GtkButton *EditAreaSettingButton;
    GtkButton *ExtensionsButton;
    GtkGrid *TabButtonBoxSeparator;
    GtkButton *CloseButton;

    GtkStack *Stack;// right-hand side
    GtkBox *EditAreaSettingPage;
    GtkBox *ExtensionsPage;
};

#endif
