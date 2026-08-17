#include "DiagnosticPopover.h"
#include "editarea/DiagnosticTool.h"
#include "editarea/EditArea.h"

#include <cstdio>
#include <gdk/gdk.h>
#include <glib-object.h>
#include <gtk/gtk.h>

DiagnosticPopover::DiagnosticPopover(){
    m_popover = GTK_POPOVER(gtk_popover_new());
    m_messageLabel = GTK_LABEL(gtk_label_new("message here"));
    gtk_label_set_single_line_mode(m_messageLabel, false);
    gtk_popover_set_autohide(m_popover, false);
    gtk_popover_set_child(m_popover, GTK_WIDGET(m_messageLabel));
    gtk_popover_set_position(m_popover, GTK_POS_BOTTOM);
    gtk_popover_set_has_arrow(m_popover, false);
    gtk_widget_remove_css_class(GTK_WIDGET(m_popover), "background");
    gtk_widget_add_css_class(GTK_WIDGET(m_popover), "diagnostic-popover");
    gtk_label_set_wrap(m_messageLabel, true);
}

DiagnosticPopover::~DiagnosticPopover(){
    //
}

void DiagnosticPopover::Hide(){
    gtk_popover_popdown(m_popover);
    m_showingDiagnostic = nullptr;
}

void DiagnosticPopover::Show(const Diagnostic& diagnostic, const GdkRectangle& rec){
    if (!m_target) {
        printf("DiagnosticPopover error: no target EditArea\n");
        return;
    }

    gtk_label_set_label(m_messageLabel, diagnostic.message.c_str());
    gtk_popover_set_pointing_to(m_popover, &rec);
    if (m_showingDiagnostic) {
        if (m_showingDiagnostic->severity != diagnostic.severity) {
            const std::string& oldsev = DiagnosticTool::StringifySeverity(m_showingDiagnostic->severity);
            gtk_widget_remove_css_class(GTK_WIDGET(m_popover), oldsev.c_str());
            const std::string& sev = DiagnosticTool::StringifySeverity(diagnostic.severity);
            if (sev != "unknown") {
                gtk_widget_add_css_class(GTK_WIDGET(m_popover), sev.c_str());
            }
        }
    } else {
        const std::string& sev = DiagnosticTool::StringifySeverity(diagnostic.severity);
        if (sev != "unknown") {
            gtk_widget_add_css_class(GTK_WIDGET(m_popover), sev.c_str());
        }
    }

    m_showingDiagnostic = &diagnostic;
    gtk_popover_popup(m_popover);
}

void DiagnosticPopover::SetTarget(EditArea* target){
    m_target = target;
    g_object_ref(m_popover);
    gtk_widget_set_parent(GTK_WIDGET(m_popover), GTK_WIDGET(m_target->GetTextView()));
    g_object_unref(m_popover);
}

const Diagnostic* DiagnosticPopover::GetShowingDiagnostic(){
    return m_showingDiagnostic;
}
