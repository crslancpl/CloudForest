#ifndef DIAGNOSTICPANEL_H_
#define DIAGNOSTICPANEL_H_

#include "components/Flyout.h"

#include <gtk/gtk.h>
#include <unordered_set>
// forward declare
class EditArea;

typedef struct AppUI AppUI ;

class DiagnosticPanel : public Flyout{
public:
    DiagnosticPanel(AppUI& appui);

    void ShowFor(EditArea* target);

private:
    EditArea *m_target;
    std::unordered_set<GtkButton*> m_diagnBtnList;
    GtkBox* m_diagnBtnBox;
};

#endif
