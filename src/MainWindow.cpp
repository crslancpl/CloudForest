#include <cstddef>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <gtk/gtkcssprovider.h>
#include <gtk/gtkstyleprovider.h>
#include <glib/gfileutils.h>


#include "MainWindow.h"

#include "DataTypes.h"
#include "CssLoader.h"
#include "FileManager.h"
#include "HeaderBar.h"


MainWindow ThisWindow;

void NewWindow (GtkApplication *app, gpointer user_data){

  ThisWindow = MainWindow();
  /* Construct a GtkBuilder instance and load our UI description */
  GtkBuilder *builder = gtk_builder_new ();

  /* Constructing MainWindow */
  gtk_builder_add_from_file (builder, "UI/MainWindow.ui", NULL);
  ThisWindow.Window = GTK_WINDOW(gtk_builder_get_object (builder, "MainWindow"));
  ThisWindow.App = app;
  gtk_window_set_default_size(ThisWindow.Window, 800, 600);
  gtk_window_set_application (ThisWindow.Window, app);

  FilePanel p;
  gtk_window_set_child(ThisWindow.Window, GTK_WIDGET(p.Grid));

  LoadCssFromPath("UI/FilePanel.css");
  LoadCssFromPath("UI/MainWindow.css");
  LoadCssFromPath("UI/HeaderBar.css");
  LoadCssFromPath("UI/EditArea.css");

   // share a builder so it can be unref later at once

  gtk_widget_set_visible (GTK_WIDGET(ThisWindow.Window), TRUE);

  /* Load Headerbar */
  HeaderBar *hb = LoadHeaderBar(builder, ThisWindow.App);
  gtk_window_set_titlebar(GTK_WINDOW(ThisWindow.Window), GTK_WIDGET(hb->HeaderBar));

  InitFileManager(ThisWindow.Window);

  /* We do not need the builder any more */
  g_object_unref (builder);
}
