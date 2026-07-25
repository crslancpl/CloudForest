#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "Window.h"
#include "headerbar/Headerbar.h"

#include <gtk/gtk.h>
#include <memory>
#include <vector>

class CfContent;
class CfLayout;

typedef struct AppUI AppUI;

class MainWindow : public Window{
public:
    MainWindow(AppUI &appui);
    ~MainWindow();

    void SetHeaderBar(std::unique_ptr<HeaderBar> headerbar);
    void Insert(std::unique_ptr<CfContent> content);

private:
    std::unique_ptr<CfLayout> m_layout;
    std::unique_ptr<HeaderBar> m_headerBar;
    std::vector<std::unique_ptr<Window>> m_childWindow;
};

#endif
