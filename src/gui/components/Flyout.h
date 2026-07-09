#ifndef FLYOUT_H_
#define FLYOUT_H_

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

class Flyout {
public:
    Flyout(GtkWindow* patent);

    void SetSize(unsigned int width, unsigned int height);
    void SetChild(GtkWidget* child);

    void Show();
    void Hide();

protected:
    GtkWindow* m_flyoutWindow;
    GtkEventController* m_focusEventCtrl;
};

#endif
