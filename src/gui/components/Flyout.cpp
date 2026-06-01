#include "Flyout.h"

#include <glib-object.h>
#include <gtk/gtk.h>

static void OnFocusLost(GtkEventControllerFocus* self, Flyout* parent){
    parent->Hide();
}

Flyout::Flyout(GtkWindow* parent){
    m_flyoutWindow = GTK_WINDOW(gtk_window_new());
    gtk_window_set_transient_for(m_flyoutWindow, parent);
    m_focusEventCtrl = gtk_event_controller_focus_new();
    gtk_widget_add_controller(GTK_WIDGET(m_flyoutWindow), m_focusEventCtrl);
    g_signal_connect(m_focusEventCtrl, "leave", G_CALLBACK(OnFocusLost), this);
    gtk_window_set_decorated(m_flyoutWindow, false);
}

void Flyout::SetSize(unsigned int width, unsigned int height){
    gtk_window_set_default_size(m_flyoutWindow, width, height);
}
void Flyout::SetChild(GtkWidget* child){
    gtk_window_set_child(m_flyoutWindow, child);
}

void Flyout::Show(){
    gtk_widget_set_visible(GTK_WIDGET(m_flyoutWindow), true);
}

void Flyout::Hide(){
    gtk_widget_set_visible(GTK_WIDGET(m_flyoutWindow), false);
}
