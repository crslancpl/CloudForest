#include "SettingPanel.h"

#include "src/gui/layouts/layout/CfLayout.h"
#include "src/gui/components/CfContent.h"
#include "src/gui/Gui_if.h"
#include "src/gui/windows/MainWindow.h"
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

static void CloseClicked(GtkButton *self, gpointer data){
    gtk_widget_set_visible(GTK_WIDGET(gui::g_settingpanel->GetWindowWidget()), false);
}

static void SwitchTab(GtkButton *self, GtkBox* page){
    gui::g_settingpanel->SwitchPage();
}


SettingPanel::SettingPanel(){
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

    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/SettingPanel.ui");

    m_window = GTK_WINDOW(gtk_builder_get_object(builder, "SettingWindow"));
    m_baseLayout = new CfLayout(GTK_ORIENTATION_HORIZONTAL);
    gtk_window_set_child(m_window, GTK_WIDGET(m_baseLayout->GetBaseWidget()));
    gtk_window_set_decorated(m_window, false);
    gtk_window_set_transient_for(m_window, gui::g_mainwindow->GetGtkWindow());

    BindTabButtons(builder);

    auto tabbutboxcontent = cfcontent::PackAsCfContent(GTK_WIDGET(m_tabButtonBox));
    tabbutboxcontent->SetDefaultSize(200, 0);
    m_baseLayout->InsertChild(tabbutboxcontent);

    m_stack = GTK_STACK(gtk_stack_new());
    gtk_widget_set_hexpand(GTK_WIDGET(m_stack), true);
    gtk_widget_add_css_class(GTK_WIDGET(m_stack), "SettingPage");
    auto stack = cfcontent::PackAsCfContent(GTK_WIDGET(m_stack));
    stack->SetHorizontalExpand(false);
    stack->SetVerticalExpand(true);
    stack->SetDefaultSize(270, 270);

    m_baseLayout->InsertChild(stack);
    g_object_unref(builder);
}

void SettingPanel::BindTabButtons(GtkBuilder* builder){
    /*
     * The left hand side of the setting panel
     */
    m_tabButtonBox = GTK_BOX(gtk_builder_get_object(builder, "TabButtonBox"));

    gtk_widget_set_hexpand(GTK_WIDGET(m_tabButtonBox), false);

    m_closeButton = GTK_BUTTON(gtk_builder_get_object(builder,"CloseButton"));

    gtk_widget_add_css_class(GTK_WIDGET(m_window), "SettingPanel");
    gtk_widget_add_css_class(GTK_WIDGET(m_tabButtonBox), "SettingTabButtonBox");
    gtk_widget_add_css_class(GTK_WIDGET(m_closeButton), "CloseButton");

    g_signal_connect(m_closeButton, "clicked", G_CALLBACK(CloseClicked), nullptr);
}

GtkWindow* SettingPanel::GetWindowWidget(){
    return m_window;
}

void SettingPanel::Show(){
    /*
     * Everytime the setting window is opened, we will resize the setting window to 2/3 in
     * both width and height of the main window. And the Stack will take 70% of the area
     * of the setting panel.
     */
    GtkWidget *mainwindow = GTK_WIDGET(gui::g_mainwindow->GetGtkWindow());
    int w = gtk_widget_get_width(mainwindow)/1.5;
    int h = gtk_widget_get_height(mainwindow)/1.5;

    gtk_widget_set_size_request(GTK_WIDGET(m_tabButtonBox), w*0.3, 0);
    gtk_window_set_default_size(m_window, w,h);
    gtk_widget_set_visible(GTK_WIDGET(m_window), true);
}

void SettingPanel::SwitchPage(){
    //
}

void SettingPanel::AddTabButton(const char* name){
    gtk_box_prepend(m_tabButtonBox, gtk_button_new_with_label(name));
}
