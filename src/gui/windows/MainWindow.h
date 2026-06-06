#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Window.h"

#include <gtk/gtk.h>

class CfContent;
class HeaderBar;
class CfLayout;

class MainWindow : public Window{
public:
    MainWindow(GtkApplication* app);
    ~MainWindow();

    void Insert(CfContent *content);

private:
    HeaderBar *m_headderBar;

    CfLayout *m_layout;
};

#endif
