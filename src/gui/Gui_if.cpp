#include "Gui_if.h"

#include "editarea/EditArea_if.h"
#include "filepanel/FilePanel.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "style/Style.h"
#include "layouts/tab/CfTab_if.h"
#include "layouts/tab/CfTabLayout.h"
#include <gtk/gtk.h>

static MainWindow* current_main_window;
static FilePanel* current_file_panel;
static SettingPanel* current_setting_panel;

namespace gui{

void Init(){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");

    current_main_window = new MainWindow();// freed on app closed
    current_file_panel = new FilePanel();// freed on app closed
    current_setting_panel = new SettingPanel();// freed on app closed
}

void Start(){
    current_main_window->Insert(current_file_panel);
    current_main_window->Insert(tablayout::NewTabLayout());

    editarea::CreateEmptyFile();
    current_main_window->Show();
}

MainWindow* GetMainWindow(){
    return current_main_window;
}

FilePanel* GetFilePanel(){
    return current_file_panel;
}

SettingPanel* GetSettingPanel(){
    return current_setting_panel;
}


}// namespace gui
