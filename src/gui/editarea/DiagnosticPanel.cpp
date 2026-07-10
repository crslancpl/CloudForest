#include "DiagnosticPanel.h"

#include "AppUI.h"
#include "datatypes/common.h"
#include "datatypes/lsp.h"
#include "components/Flyout.h"
#include "editarea/EditArea.h"
#include "Gui_if.h"
#include "windows/MainWindow.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <string>
#include <unordered_set>

DiagnosticPanelItem::DiagnosticPanelItem(const Diagnostic& diagnostic){
    //
    ZPosition startpos = diagnostic.range.start;
    std::string location = "> line " + std::to_string(startpos.line) + " col " + std::to_string(startpos.column);
    m_locationLabel = GTK_LABEL(gtk_label_new(location.c_str()));
    gtk_label_set_xalign(m_locationLabel, 0);
    gtk_widget_add_css_class(GTK_WIDGET(m_locationLabel), "location-label");

    m_messageLabel = GTK_LABEL(gtk_label_new(diagnostic.message));
    gtk_label_set_xalign(m_messageLabel, 0);
    gtk_label_set_wrap(m_messageLabel, true);
    gtk_widget_add_css_class(GTK_WIDGET(m_messageLabel), "message-label");

    m_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_box_append(m_box, GTK_WIDGET(m_locationLabel));
    gtk_box_append(m_box, GTK_WIDGET(m_messageLabel));

    m_button = GTK_BUTTON(gtk_button_new());
    gtk_button_set_child(m_button, GTK_WIDGET(m_box));

    switch (diagnostic.severity) {
        case 1:
            gtk_widget_add_css_class(GTK_WIDGET(m_button), "error");
            break;
        case 2:
            gtk_widget_add_css_class(GTK_WIDGET(m_button), "warning");
            break;
        case 3:
            gtk_widget_add_css_class(GTK_WIDGET(m_button), "info");
            break;
        case 4:
            gtk_widget_add_css_class(GTK_WIDGET(m_button), "hint");
            break;
        default:
            break;
    }
}

DiagnosticPanelItem::~DiagnosticPanelItem(){
    //
}


DiagnosticPanel::DiagnosticPanel(AppUI& appui) : Flyout(appui.mainWindow->GetGtkWindow()){
    gtk_widget_add_css_class(GTK_WIDGET(m_flyoutWindow), "diagnostic-panel");
    m_diagnItemBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 4));

    m_scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    gtk_scrolled_window_set_child(m_scrolledWindow, GTK_WIDGET(m_diagnItemBox));

    this->SetSize(400, 300);
    this->SetChild(GTK_WIDGET(m_scrolledWindow));
}

GtkWidget* DiagnosticPanelItem::GetBaseWidget(){
    return GTK_WIDGET(m_button);
}

void DiagnosticPanel::ShowFor(EditArea* target){
    m_target = target;
    this->Clear();

    const std::unordered_set<std::unique_ptr<Diagnostic>>& diagns = target->GetDiagnosticsList();

    if(diagns.empty()){
        return;
    }

    for (const std::unique_ptr<Diagnostic>& diagn : diagns){
        DiagnosticPanelItem* item = new DiagnosticPanelItem(*diagn);// delete on Clear()
        gtk_box_prepend(m_diagnItemBox, GTK_WIDGET(item->GetBaseWidget()));
        m_itemSet.insert(item);
    }

    this->Show();
}

void DiagnosticPanel::Clear(){
    for (DiagnosticPanelItem* item : m_itemSet) {
        //
        gtk_box_remove(m_diagnItemBox, item->GetBaseWidget());
        delete item;
    }

    m_itemSet.clear();
}
