#include "SettingPanel.h"

#include "AppUI.h"
#include "headerbar/Headerbar.h"
#include "layouts/layout/CfLayout.h"
#include "components/CfContent.h"
#include "settingpanel/ExtensionPage.h"
#include "settingpanel/SettingPage.h"
#include "windows/MainWindow.h"
#include "windows/Window.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

static void OnCloseClicked(GtkButton *self, SettingPanel* parent){
    parent->Hide();
}

static void OnSwitcherClicked(GtkButton *self, SettingPanel* parent){
    parent->SwitchPage(gtk_button_get_label(self));
}

SettingPanel::SettingPanel(AppUI& appui): Window(false)
    ,m_appUI(appui)
    {
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
    this->BindUI(builder);

    m_baseLayout = new CfLayout(GTK_ORIENTATION_HORIZONTAL);// freed on app closed

    auto stack = cfcontent::PackAsCfContent(GTK_WIDGET(m_stack));
    auto tabbutboxcontent = cfcontent::PackAsCfContent(GTK_WIDGET(m_tabButtonBox));

    gtk_window_set_child(m_window, GTK_WIDGET(m_baseLayout->GetBaseWidget()));

    tabbutboxcontent->SetDefaultSize(200, 0);
    m_baseLayout->InsertChild(*tabbutboxcontent);
    m_baseLayout->InsertChild(*stack);
    stack->SetHorizontalExpand(true);
    stack->SetVerticalExpand(true);

    this->AddPage("Extension", new ExtensionPage());//freed on app closed

    g_object_unref(builder);
}

void SettingPanel::Show(){
    /*
     * Everytime the setting window is opened, we will resize the setting window to 2/3 in
     * both width and height of the main window. And the Stack will take 70% of the area
     * of the setting panel.
     */
    GtkWidget *mainwindow = GTK_WIDGET(m_appUI.mainWindow->GetGtkWindow());
    int w = gtk_widget_get_width(mainwindow)/1.5;
    int h = gtk_widget_get_height(mainwindow)/1.5;

    if (w* 0.3 > 200) {
        gtk_widget_set_size_request(GTK_WIDGET(m_tabButtonBox), 200, 0);
    }else{
        gtk_widget_set_size_request(GTK_WIDGET(m_tabButtonBox), w*0.3, 0);
    }

    gtk_window_set_default_size(m_window, w,h);
    gtk_widget_set_visible(GTK_WIDGET(m_window), true);
}

void SettingPanel::SwitchPage(const char* name){
    gtk_stack_set_visible_child_name(m_stack, name);
}

void SettingPanel::AddTabButton(const char* name){
    gtk_box_prepend(m_tabButtonBox, gtk_button_new_with_label(name));
}

void SettingPanel::AddPage(const char* name, SettingPage* page){
    GtkButton* switcher = GTK_BUTTON(gtk_button_new_with_label(name));
    gtk_widget_add_css_class(GTK_WIDGET(switcher), "tab-btn");
    gtk_box_prepend(m_tabButtonBox, GTK_WIDGET(switcher));
    gtk_widget_set_hexpand(GTK_WIDGET(switcher), true);
    g_signal_connect(switcher, "clicked", G_CALLBACK(OnSwitcherClicked), this);
    gtk_stack_add_named(m_stack, page->GetBaseWidget(), name);
}

// private
void SettingPanel::BindUI(GtkBuilder* builder){
    m_window = GTK_WINDOW(gtk_builder_get_object(builder, "setting-window"));
    m_tabButtonBox = GTK_BOX(gtk_builder_get_object(builder, "tab-btn-box"));
    m_closeButton = GTK_BUTTON(gtk_builder_get_object(builder, "close-btn"));
    m_stack = GTK_STACK(gtk_stack_new());


    gtk_window_set_decorated(m_window, false);
    gtk_window_set_transient_for(m_window, m_appUI.mainWindow->GetGtkWindow());
    gtk_widget_set_hexpand(GTK_WIDGET(m_tabButtonBox), false);
    gtk_widget_set_hexpand(GTK_WIDGET(m_stack), true);

    gtk_widget_add_css_class(GTK_WIDGET(m_stack), "setting-page");
    gtk_widget_add_css_class(GTK_WIDGET(m_window), "setting-panel");
    gtk_widget_add_css_class(GTK_WIDGET(m_tabButtonBox), "tab-btn-box");
    gtk_widget_add_css_class(GTK_WIDGET(m_closeButton), "close-btn");

    g_signal_connect(m_closeButton, "clicked", G_CALLBACK(OnCloseClicked), this);
}
