#ifndef DIAGNOSTICPOPOVER_H_
#define DIAGNOSTICPOPOVER_H_

#include <gdk/gdk.h>
#include <gtk/gtk.h>

typedef struct Diagnostic Diagnostic;
class EditArea;
class AppUI;

class DiagnosticPopover{
public:
    DiagnosticPopover();
    ~DiagnosticPopover();

    void Hide();
    void Show(const Diagnostic& diagnostic, const GdkRectangle& rec);

    void SetTarget(EditArea* target);

    const Diagnostic* GetShowingDiagnostic();

private:
    GtkPopover* m_popover;
    GtkLabel* m_messageLabel;
    GtkTextIter m_iter;// for getting rectangle

    const Diagnostic* m_showingDiagnostic = nullptr;

    //from parent
    EditArea* m_target = nullptr;
};

#endif
