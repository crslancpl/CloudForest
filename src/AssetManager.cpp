#include "AssetManager.h"

#include <gtk/gtk.h>

//UI (not implemented)
GResource *UIResources;

bool LoadUIRes(){
    GError *e;
    UIResources = g_resource_load("gresources/ui.gresource.xml", &e);
    if(e != nullptr){

        return false;
    }else{
        g_print("true");
        g_resources_register(UIResources);
        return true;
    }
}

//Settings
GSettings *Settings;
void LoadSettings(){
    Settings = g_settings_new_with_path("ide.cf", "gres/gschemas.compiled");
    char* c = nullptr;
    g_settings_get(Settings, "editor-tab-type", "%s", c);
    g_print("%s", c);
}
