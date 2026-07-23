#ifndef DIAGNOSTICPANEL_H_
#define DIAGNOSTICPANEL_H_

#include "components/CfComponent.h"
#include "components/Flyout.h"
#include "datatypes/lsp.h"

#include <gtk/gtk.h>
#include <memory>
#include <unordered_set>
// forward declare
class DiagnosticPanel;
class EditArea;

typedef struct AppUI AppUI ;



class DiagnosticPanelItem : public CfComponent{
public:
    DiagnosticPanelItem(const Diagnostic& diagnostic, DiagnosticPanel& parent);
    ~DiagnosticPanelItem();

    GtkWidget * GetBaseWidget() override;

private:
    DiagnosticPanel& m_parent;
    GtkButton* m_button;
    GtkBox* m_box;
    GtkLabel* m_locationLabel;
    GtkLabel* m_messageLabel;// diagnostic text
};



class DiagnosticPanel : public Flyout{
public:
    DiagnosticPanel(AppUI& appui);

    void ShowFor(EditArea* target);
    void Clear();

private:
    EditArea *m_target;
    std::unordered_set<std::unique_ptr<DiagnosticPanelItem>> m_itemSet;
    GtkScrolledWindow* m_scrolledWindow;
    GtkBox* m_diagnItemBox;
};

#endif
