#include "DiagnosticPanel.h"

#include "datatypes/lsp.h"
#include "src/gui/components/Flyout.h"
#include "src/gui/editarea/EditArea.h"
#include "src/gui/Gui_if.h"
#include "src/gui/windows/MainWindow.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <unordered_set>

static DiagnosticPanel* diag_panel = nullptr;

DiagnosticPanel::DiagnosticPanel() : Flyout(gui::GetMainWindow()->GetGtkWindow()){
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

void OpenDiagnosticPanelForEditArea(EditArea* target){
    if (!diag_panel) {
        diag_panel = new DiagnosticPanel();
    }

    diag_panel->ShowFor(target);
}

void CloseDiagnosticPanel(){
    if(!diag_panel) return;
    diag_panel->Hide();
}
