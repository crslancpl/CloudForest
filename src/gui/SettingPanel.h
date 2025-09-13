#ifndef SETTINGPANEL_H_
#define SETTINGPANEL_H_

#include <gtk/gtk.h>
#include <gtk/gtkdropdown.h>

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

    /*
     * Edit area setting page
     */
    GtkDropDown* TabBehaviorChooser;//determine whether showing tab or space when the tab button is clicked
    GtkSpinButton* TabSizeSetter;
};

#endif
