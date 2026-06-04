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

MainWindow::MainWindow(){
    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/MainWindow.ui");
    m_window = GTK_WINDOW(gtk_builder_get_object(builder, "main-window"));

    m_headderBar = new HeaderBar();
    gtk_window_set_titlebar(m_window, m_headderBar->GetBaseWidget());

    m_layout = new CfLayout(GTK_ORIENTATION_HORIZONTAL);
    gtk_window_set_child(m_window, GTK_WIDGET(m_layout->GetBaseWidget()));

    gtk_window_set_default_size(m_window, 1000, 600);
    gtk_window_set_hide_on_close(m_window, false);

    gtk_application_add_window(session::GetApplication(), m_window);
}

MainWindow::~MainWindow(){
    //
}

void MainWindow::Show(){
    gtk_widget_set_visible(GTK_WIDGET(m_window), true);
}

void MainWindow::Insert(CfContent *content){
    m_layout->InsertChild(content);
}

GtkWindow* MainWindow::GetGtkWindow(){
    return m_window;
}
