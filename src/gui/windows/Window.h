#ifndef WINDOW_H_
#define WINDOW_H_

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

class Window{
public:
    Window(bool constructwindow);// false if you will construct the GtkWindow yourself
    ~Window();

    void SetChildWidget(GtkWidget* widget);
    GtkWidget* GetChildWidget();

    GtkWindow* GetGtkWindow();

    virtual void Show();
    virtual void Hide();

protected:
    GtkWindow *m_window;
    GtkWidget *m_childWidget;
};

#endif
