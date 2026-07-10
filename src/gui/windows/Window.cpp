#include "Window.h"

#include <glib-object.h>
#include <gtk/gtk.h>

Window::Window(bool constructwindow){
    if (constructwindow) {
        m_window = GTK_WINDOW(gtk_window_new());
        g_object_ref(m_window);
    }
}

Window::~Window(){
    g_object_unref(m_window);
}

void Window::SetChildWidget(GtkWidget* widget){
    m_childWidget = widget;
}

GtkWidget* Window::GetChildWidget(){
    return m_childWidget;
}

GtkWindow* Window::GetGtkWindow(){
    return m_window;
}

// virtual
void Window::Show(){
    gtk_widget_set_visible(GTK_WIDGET(m_window), true);
}

void Window::Hide(){
    gtk_widget_set_visible(GTK_WIDGET(m_window), false);
}
