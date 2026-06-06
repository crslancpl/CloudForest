/*
 * There is only one MainWindow for an app. And the mainwindow for the app is
 * stored in guiCore.
 */

#include "MainWindow.h"

#include <gtk/gtk.h>

#include "../layouts/layout/CfLayout.h"
#include "../headerbar/Headerbar.h"
#include "src/Session.h"
#include "src/gui/Gui_if.h"
#include "src/gui/windows/Window.h"

MainWindow::MainWindow(GtkApplication* app) : Window(true){
    gtk_widget_add_css_class(GTK_WIDGET(m_window), "main-window");
    m_headderBar = new HeaderBar(app);// freed on MainWindow deleted(app closed)
    gtk_window_set_titlebar(m_window, m_headderBar->GetBaseWidget());

    m_layout = new CfLayout(GTK_ORIENTATION_HORIZONTAL);// freed on MainWindow deleted(app closed)
    gtk_window_set_child(m_window, GTK_WIDGET(m_layout->GetBaseWidget()));

    gtk_window_set_default_size(m_window, 1000, 600);
    gtk_window_set_hide_on_close(m_window, false);

    gtk_application_add_window(app, m_window);
}

MainWindow::~MainWindow(){
    delete m_headderBar;
    delete m_layout;
}

void MainWindow::Insert(CfContent *content){
    m_layout->InsertChild(content);
}
