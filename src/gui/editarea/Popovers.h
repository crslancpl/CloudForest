#ifndef POPOVERS_H_
#define POPOVERS_H_

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

#endif
