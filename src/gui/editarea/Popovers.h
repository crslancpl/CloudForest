#ifndef POPOVERS_H_
#define POPOVERS_H_

#include "datatypes/lsp.h"
#include "editarea/EditArea.h"
#include <gdk/gdk.h>
#include <gtk/gtk.h>

//forward declaration
typedef struct Suggestion Suggestion;


class TipPopover{
public:
    TipPopover(const GdkRectangle &cursorrect);
    ~TipPopover();

    void ShowContent(const char* content);
    void Hide();

private:
    const GdkRectangle &m_cursorRect;
    GtkLabel *m_tipLabel;
    GtkPopover *m_popover;
};


class DiagnosticPopover{
public:
    DiagnosticPopover(GtkTextView* textview);

    void Hide();
    void Show(const Diagnostic& diagnostic, double xpos, double ypos);

    const Diagnostic* GetShowingDiagnostic();

public:
    GtkPopover* m_popover;
    GtkLabel* m_messageLabel;
    GtkTextIter m_iter;// for getting rectangle
    GdkRectangle m_rectangle;

    int m_height;
    const Diagnostic* m_showingDiagnostic;

    //from parent
    GtkTextView* m_targetTextView;
};

#endif
