#include <gtk/gtk.h>

#include "MainWindow.h"


int main (int argc,char *argv[]){
#ifdef GTK_SRCDIR
  g_chdir (GTK_SRCDIR);
#endif
  GtkApplication *app = gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);
  g_signal_connect (app, "activate", G_CALLBACK (NewWindow), nullptr);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
