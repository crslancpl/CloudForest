#include "Popovers.h"

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
