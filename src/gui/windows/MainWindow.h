#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Window.h"
#include "headerbar/Headerbar.h"

#include <gtk/gtk.h>
#include <memory>

class CfContent;
class CfLayout;

typedef struct AppUI AppUI;

class MainWindow : public Window{
public:
    MainWindow(AppUI &appui);
    ~MainWindow();

    void SetHeaderBar(HeaderBar& headerbar);
    void Insert(CfContent& content);

private:
    std::unique_ptr<CfLayout> m_layout;
};

#endif
