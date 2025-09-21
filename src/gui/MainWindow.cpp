/*
 * There is only one MainWindow for an app. And the mainwindow for the app is
 * stored in guiCore.
 */

#include "MainWindow.h"
#include "CFLayout.h"
#include "HeaderBar.h"
#include "guiCore.h"
#include "../Global.h"
#include <memory>

static void SeparatorDragUpdate(GtkGestureDrag* self, gdouble x, gdouble y, gpointer user_data){
    //resize the file panel
}

void MainWindow::Init(){
    GtkBuilder *builder = gtk_builder_new_from_file("UI/MainWindow.ui");
    Window = GTK_WINDOW(gtk_builder_get_object(builder, "MainWindow"));
    Layout.Init(GTK_ORIENTATION_HORIZONTAL);
    gtk_window_set_child(Window, GTK_WIDGET(Layout.BaseBox));

    gtk_window_set_default_size(Window, 1000, 600);
    if(global::GtkApp == nullptr){
        g_print("null gtkapp\n");
    }
    gtk_application_add_window(global::GtkApp, Window);
}

void MainWindow::SetHeaderBar(GtkWidget *headerbar){
    gtk_window_set_titlebar(Window, GTK_WIDGET(headerbar));
}

void MainWindow::Show(){
    gtk_widget_set_visible(GTK_WIDGET(Window), true);
}
