#include "MainWindow_if.h"
#include "MainWindow.h"
#include "../headerbar/Headerbar.h"
#include "../Gui_if.h"

void mainwindow::Construct(){
    if(!gui::g_mainwindow){
        gui::g_mainwindow = new MainWindow();
        headerbar::Construct();
        gui::g_mainwindow->SetHeaderBar(gui::g_headerbar);
        gui::g_mainwindow->Show();
    }
}
