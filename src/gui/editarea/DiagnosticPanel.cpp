#include "DiagnosticPanel.h"

#include "AppUI.h"
#include "datatypes/lsp.h"
#include "components/Flyout.h"
#include "editarea/EditArea.h"
#include "Gui_if.h"
#include "windows/MainWindow.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <unordered_set>

DiagnosticPanel::DiagnosticPanel(AppUI& appui) : Flyout(appui.mainWindow->GetGtkWindow()){
    this->SetSize(200, 100);
    m_diagnBtnBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));
    this->SetChild(GTK_WIDGET(m_diagnBtnBox));
}

void DiagnosticPanel::ShowFor(EditArea* target){
    m_target = target;
    for(auto diagnbtn : m_diagnBtnList){
        gtk_box_remove(m_diagnBtnBox, GTK_WIDGET(diagnbtn));
    }
    m_diagnBtnList.clear();
    const std::unordered_set<Diagnostic*>& diagns = target->GetDiagnosticsList();

    if(diagns.empty()){
        return;
    }

    for (Diagnostic* diagn : diagns){
        GtkButton* btn = GTK_BUTTON(gtk_button_new_with_label(diagn->message));
        gtk_box_append(m_diagnBtnBox, GTK_WIDGET(btn));
        m_diagnBtnList.insert(btn);
    }

    this->Show();
}
