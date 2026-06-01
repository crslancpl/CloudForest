#ifndef DIAGNOSTICPANEL_H_
#define DIAGNOSTICPANEL_H_

#include "src/gui/components/Flyout.h"

#include <gtk/gtk.h>
#include <unordered_set>
// forward declare
class EditArea;

class DiagnosticPanel : public Flyout{
public:
    DiagnosticPanel();

    void ShowFor(EditArea* target);

private:
    EditArea *m_target;
    std::unordered_set<GtkButton*> m_diagnBtnList;
    GtkBox* m_diagnBtnBox;
};

void OpenDiagnosticPanelForEditArea(EditArea* target);
void CloseDiagnosticPanel();

#endif
