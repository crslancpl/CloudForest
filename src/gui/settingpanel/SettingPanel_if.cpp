#include "SettingPanel_if.h"

#include "SettingPanel.h"
#include "src/gui/Gui_if.h"

void settingpanel::Construct(){
    if(!gui::g_settingpanel){
        gui::g_settingpanel = new SettingPanel();
    }
}

void settingpanel::AddSetting(const char *name){
    gui::g_settingpanel->AddTabButton(name);
}
