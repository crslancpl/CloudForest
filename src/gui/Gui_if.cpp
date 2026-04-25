/*
 * now the gui parts of the app will not interact with core directly
 */
#include "Gui_if.h"

#include "editarea/EditArea_if.h"
#include "editarea/LangPanel.h"
#include "editarea/LspPopovers_if.h"
#include "filepanel/FilePanel_if.h"
#include "filepanel/FilePanel.h"
#include "src/gui/settingpanel/SettingPanel_if.h"
#include "windows/MainWindow_if.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "style/Style.h"
#include "layouts/tab/CfTab_if.h"
#include "layouts/tab/CfTabLayout.h"



MainWindow* gui::g_mainwindow;
SettingPanel* gui::g_settingpanel;
HeaderBar* gui::g_headerbar;
FilePanel* gui::g_filepanel;


void gui::Init(){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");

    mainwindow::Construct();
    filepanel::Construct();
    langpanel::Construct();
    lsppopovers::Construct();
    settingpanel::Construct();
}

void gui::Start(){
    gui::g_mainwindow->Insert(gui::g_filepanel);
    gui::g_mainwindow->Insert(tablayout::NewTabLayout());

    editarea::CreateEmptyFile();

    gui::g_mainwindow->Show();
}
