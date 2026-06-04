#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtk/gtk.h>

class CfContent;
class HeaderBar;
class CfLayout;

class MainWindow{
public:
    MainWindow();
    ~MainWindow();

    void Show();
    void Insert(CfContent *content);
    GtkWindow* GetGtkWindow();

private:
    HeaderBar *m_headderBar;

    GtkWindow *m_window;
    CfLayout *m_layout;
};

#endif
