#include "SettingPanel.h"
#include "guiCore.h"
#include "MainWindow.h"
//#include "FilePanel.h"
#include <gtk/gtk.h>
#include <gtk/gtkdropdown.h>

static void CloseClicked(GtkButton *self, gpointer data){
    gtk_widget_set_visible(GTK_WIDGET(gui::AppSettingPanel.Window), false);
}

static void SwitchTab(GtkButton *self, GtkBox* page){
    gtk_stack_set_visible_child(gui::AppSettingPanel.Stack, GTK_WIDGET(page));
}

void SettingPanel::Init(){
    /*
     * The ui is constructed from UI/SettingPanel.ui
     *
     * SettingWindow is used as the flyout. It contains a BaseBox, which
     * separates the TabButtonBox and the Stack.
     *
     * Buttons inside TabButtonBox let you switch between different setting pages or close the setting window.
     *
     * Stack is the place you can adjust the settings. All setting pages were currently a GtkBox as we expect
     * the setting to look like a list.
     */

    GtkBuilder *builder = gtk_builder_new_from_file("UI/SettingPanel.ui");

    Window = GTK_WINDOW(gtk_builder_get_object(builder, "SettingWindow"));
    BaseBox = GTK_BOX(gtk_builder_get_object(builder, "BaseBox"));

    TabButtonBox = GTK_BOX(gtk_builder_get_object(builder, "TabButtonBox"));
    EditAreaSettingButton = GTK_BUTTON(gtk_builder_get_object(builder, "EditAreaButton"));
    ExtensionsButton = GTK_BUTTON(gtk_builder_get_object(builder, "ExtensionButton"));
    CloseButton = GTK_BUTTON(gtk_builder_get_object(builder,"CloseButton"));

    Stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    EditAreaSettingPage = GTK_BOX(gtk_builder_get_object(builder, "EditAreaSettingPage"));
    ExtensionsPage = GTK_BOX(gtk_builder_get_object(builder, "ExtensionsPage"));

    TabBehaviorChooser = GTK_DROP_DOWN(gtk_builder_get_object(builder, "tabbehavchooser"));
    TabSizeSetter = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "tabsizesetter"));

    GtkAdjustment *adj = gtk_adjustment_new(0, 0, 16, 1, 1, 0);
    gtk_spin_button_set_adjustment(TabSizeSetter, adj);

    gtk_widget_add_css_class(GTK_WIDGET(Window), "SettingPanel");
    gtk_widget_add_css_class(GTK_WIDGET(TabButtonBox), "SettingTabButtonBox");
    gtk_widget_add_css_class(GTK_WIDGET(CloseButton), "CloseButton");

    gtk_window_set_decorated(Window, false);
    gtk_window_set_transient_for(Window, gui::AppWindow.Window);

    gtk_stack_add_child(Stack, GTK_WIDGET(EditAreaSettingPage));
    gtk_stack_add_child(Stack, GTK_WIDGET(ExtensionsPage));

    g_signal_connect(EditAreaSettingButton, "clicked", G_CALLBACK(SwitchTab), EditAreaSettingPage);
    g_signal_connect(ExtensionsButton, "clicked", G_CALLBACK(SwitchTab), ExtensionsPage);
    g_signal_connect(CloseButton, "clicked", G_CALLBACK(CloseClicked), nullptr);
}

void SettingPanel::Show(){
    /*
     * Everytime the setting window is opened, we will resize the setting window to 2/3 in
     * both width and height of the main window. And the Stack will take 70% of the area
     * of the setting panel.
     */

    int w = gtk_widget_get_width(GTK_WIDGET(gui::AppWindow.Window))/1.5;
    int h = gtk_widget_get_height(GTK_WIDGET(gui::AppWindow.Window))/1.5;

    gtk_widget_set_size_request(GTK_WIDGET(Stack), w*0.7, 0);
    gtk_window_set_default_size(Window, w,h);
    gtk_widget_set_visible(GTK_WIDGET(Window), true);
}
