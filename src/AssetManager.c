#include "AssetManager.h"

GResource *UIResources;

bool LoadUIRes(){
    GError e;
    UIResources = g_resource_load("gresources/ui.gresource.xml", &e);
    if(e.code != 0){
        return false;
    }else{
        return true;
    }
}

g_resources_register(GResource *resource)
