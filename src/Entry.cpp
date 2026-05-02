/*
 * Entry.cpp consist the entry function
 */
#include "filemanagement/FileManagement_if.h"
#include "src/languages/LanguageManager_if.h"
#include "toolset/syntaxprovider/syntax_provider.h"
#include "gui/Gui_if.h"
#include "pythonbackend/python_if.h"
#include "Global.h"

#include <gtk/gtk.h>

static void AppActivated (GtkApplication *app, gpointer user_data);
static void AppClosed (GtkApplication *app, gpointer user_data);

int main (int argc,char *argv[]){
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  GtkApplication *app = gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);
  g_signal_connect (app, "activate", G_CALLBACK (AppActivated), nullptr);
  g_signal_connect(app, "shutdown", G_CALLBACK(AppClosed), nullptr);

  int status = g_application_run (G_APPLICATION (app), argc, argv);

  g_object_unref (app);

  return status;
}

static void AppActivated (GtkApplication *app, gpointer user_data){
    global::g_gtkApplication = app;
    syntaxprovider::Init();
    langmanager::Init();
    gui::Init();
    pybackend::Start();
    filemanagement::Init();
    gui::Start();
}

static void AppClosed (GtkApplication *app, gpointer user_data){
    /*
     * LSP server depends on python, so we have to stop
     * the lsp server before ending the python backend
     */

    pybackend::End();
    g_print("\nApp closed\n");
}
