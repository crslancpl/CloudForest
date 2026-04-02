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

    GtkWindow *m_window;
private:
    CfLayout *m_layout;
};

#endif
