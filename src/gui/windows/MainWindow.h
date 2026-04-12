#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtk/gtk.h>

class CfContent;
class HeaderBar;
class CfLayout;

class MainWindow{
public:
    MainWindow();
    void Show();
    void SetHeaderBar(HeaderBar *headerbar);
    void Insert(CfContent *content);
    GtkWindow* GetGtkWindow();

private:
    GtkWindow *m_window;
    CfLayout *m_layout;
};

#endif
