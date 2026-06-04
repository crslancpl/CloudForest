#include "Session.h"

#include <gtk/gtk.h>

static GtkApplication* this_application;

namespace session{

GtkApplication* GetApplication(){
    return this_application;
}


void SetApplication(GtkApplication* app){
    this_application = app;
}

} // namespace session
