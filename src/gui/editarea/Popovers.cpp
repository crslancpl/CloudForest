#include "Popovers.h"

#include "datatypes/lsp.h"

#include <gtk/gtk.h>

TipPopover::TipPopover(const GdkRectangle &cursorrect):
    m_cursorRect(cursorrect)
    {
    m_tipLabel = GTK_LABEL(gtk_label_new(""));
    m_popover = GTK_POPOVER(gtk_popover_new());
    gtk_widget_add_css_class(GTK_WIDGET(m_popover), "tippopover");
    gtk_popover_set_child(m_popover, GTK_WIDGET(m_tipLabel));
    gtk_widget_set_can_focus(GTK_WIDGET(m_popover), false);
    gtk_popover_set_position(m_popover, GTK_POS_RIGHT);
    gtk_popover_set_autohide(m_popover, false);
    gtk_popover_set_offset(m_popover, 10, 0);
}

TipPopover::~TipPopover(){
    g_object_unref(m_popover);
}

void TipPopover::ShowContent(const char* content){
    gtk_label_set_label(m_tipLabel, content);
    gtk_popover_set_pointing_to(m_popover, &m_cursorRect);
    gtk_popover_popup(m_popover);
}

void TipPopover::Hide(){
    gtk_popover_popdown(m_popover);
}






/*
 * Diagnostic Popover
 */
DiagnosticPopover::DiagnosticPopover(GtkTextView* textview)
    : m_targetTextView(textview){
    m_popover = GTK_POPOVER(gtk_popover_new());
    m_messageLabel = GTK_LABEL(gtk_label_new("message here"));
    gtk_label_set_single_line_mode(m_messageLabel, false);
    gtk_popover_set_autohide(m_popover, false);
    gtk_popover_set_child(m_popover, GTK_WIDGET(m_messageLabel));
    gtk_popover_set_position(m_popover, GTK_POS_RIGHT);
    gtk_popover_set_has_arrow(m_popover, false);
    gtk_widget_set_parent(GTK_WIDGET(m_popover), GTK_WIDGET(m_targetTextView));
}

void DiagnosticPopover::Hide(){
    gtk_popover_popdown(m_popover);
    m_showingDiagnostic = nullptr;
}

void DiagnosticPopover::Show(const Diagnostic& diagnostic, double xpos, double ypos){
    //
    gtk_label_set_label(m_messageLabel, diagnostic.message);
    int height = gtk_widget_get_height(GTK_WIDGET(m_messageLabel));
    if(m_height != height){
        gtk_popover_popdown(m_popover);
    }

    m_rectangle.x = xpos;
    m_rectangle.y = ypos + 40 + (int)(height/2);
    gtk_popover_set_pointing_to(m_popover, &m_rectangle);
    m_showingDiagnostic = &diagnostic;
    gtk_popover_popup(m_popover);
}

const Diagnostic* DiagnosticPopover::GetShowingDiagnostic(){
    return m_showingDiagnostic;
}
