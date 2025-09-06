/*
 * Core.cpp consist the entry function and the most imperitive funtions
 * that handles the app
 */

#include "Core.h"

#include <gtk/gtk.h>
#include <map>

#include "FileManager.h"
#include "cf/CFCore.h"
#include "gui/guiCore.h"
#include "PythonBackend/PythonMain.h"
#include "Global.h"

/*
 * local variables
 */

/*
 * local functions
 */
int main(int argc, char *argv[]);
static void AppActivated (GtkApplication *app, gpointer user_data);


int main (int argc,char *argv[]){
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  GtkApplication *app = gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);
  g_signal_connect (app, "activate", G_CALLBACK (AppActivated), nullptr);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

static void AppActivated (GtkApplication *app, gpointer user_data){
    global::GtkApp = app;
    pybackend::Start();
    cf::Init();
    gui::Init();
    filemanag::Init();
}


/*
 * core namespace
 */
void *core::Interact(Action *action){
    switch (action->Destination) {
    case Parts::CORE:
    break;
    case Parts::FILEMANAGER:
    filemanag::Process((FileAction*)action);
    break;
    case Parts::GUI:
    gui::Process((GUIAction*)action);
    break;
    case Parts::PYTHON:
    break;
    case Parts::CLOUDYFOREST:
    cf::Process((CFAction*)action);
    break;
    default:
    g_print("unknown action\n");
    break;
    }
    return nullptr;
}
