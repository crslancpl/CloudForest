#include "SettingPanel.h"
#include "Core.h"
#include "MainWindow.h"
#include "FilePanel.h"
#include <gtk/gtk.h>

GtkWindow *SettingWindow;
GtkButton *CloseButton;
GtkWindow *appwindow;// MainWindow
GtkBox *BaseBox;// Saperates TabButtonBox and Stack
GtkBox *TabButtonBox;// left-hand side
GtkGrid *TabButtonBoxSeparator;
GtkStack *Stack;// right-hand side

void InitSettingPanel(){
    appwindow = GetAppWindow().Window;
    SettingWindow = GTK_WINDOW(gtk_window_new());
    CloseButton = GTK_BUTTON(gtk_button_new_with_label("close"));
    BaseBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    TabButtonBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    TabButtonBoxSeparator = GTK_GRID(gtk_grid_new());
    Stack = GTK_STACK(gtk_stack_new());

    gtk_box_append(BaseBox, GTK_WIDGET(TabButtonBox));
    gtk_box_append(BaseBox, GTK_WIDGET(Stack));
    gtk_box_append(TabButtonBox, GTK_WIDGET(TabButtonBoxSeparator));
    gtk_box_append(TabButtonBox, GTK_WIDGET(CloseButton));

    gtk_widget_add_css_class(GTK_WIDGET(SettingWindow), "SettingPanel");
    gtk_widget_add_css_class(GTK_WIDGET(TabButtonBox), "SettingTabButtonBox");
    gtk_widget_add_css_class(GTK_WIDGET(CloseButton), "CloseButton");
    gtk_widget_set_hexpand(GTK_WIDGET(CloseButton),true);
    gtk_widget_set_vexpand(GTK_WIDGET(TabButtonBoxSeparator),true);

    gtk_window_set_decorated(SettingWindow, false);
    gtk_window_set_transient_for(SettingWindow, appwindow);
    gtk_window_set_child(SettingWindow, GTK_WIDGET(BaseBox));

    g_signal_connect(CloseButton, "clicked", G_CALLBACK(CloseSettingPanel), nullptr);
}

void ShowSettingPanel(){
    int w = gtk_widget_get_width(GTK_WIDGET(appwindow))/1.5;
    int h = gtk_widget_get_height(GTK_WIDGET(appwindow))/1.5;

    gtk_widget_set_size_request(GTK_WIDGET(Stack), w*0.7, 0);
    gtk_window_set_default_size(SettingWindow, w,h);
    gtk_widget_set_visible(GTK_WIDGET(SettingWindow), true);
}

void CloseSettingPanel(){
    gtk_widget_set_visible(GTK_WIDGET(SettingWindow), false);
}
