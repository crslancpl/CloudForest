#include "AssetManager.h"

#include <gtk/gtk.h>

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
