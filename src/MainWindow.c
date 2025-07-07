#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <gtk/gtkcssprovider.h>
#include <gtk/gtkstyleprovider.h>
#include <gtk/gtkwidget.h>
#include <glib/gfileutils.h>
#include <webkit/webkit.h>


#include "MainWindow.h"

#include "EditArea.h"
#include "CssLoader.h"
//#include "EditArea.h"

void NewWindow (GtkApplication *app, gpointer user_data){
  /* Construct a GtkBuilder instance and load our UI description */
  GtkBuilder *builder = gtk_builder_new ();

  /* Constructing MainWindow */
  gtk_builder_add_from_file (builder, "UI/MainWindow.ui", NULL);
  GtkWindow *window = GTK_WINDOW(gtk_builder_get_object (builder, "MainWindow"));

  gtk_window_set_default_size(window, 800, 600);
  gtk_window_set_application (window, app);

  LoadCssFromPath("UI/MainWindow.css");
  LoadCssFromPath("UI/HeaderBar.css");
  LoadCssFromPath("UI/EditArea.css");

  EditArea *ea = edit_area_new();

  gtk_widget_set_visible (GTK_WIDGET(window), TRUE);
  gtk_window_set_child(window, GTK_WIDGET(ea->BaseGrid));

  /* Load Headerbar */
  GError *e = NULL;
  gtk_builder_add_from_file(builder, "UI/HeaderBar.ui", &e);

  GObject *HeaderBar = gtk_builder_get_object(builder, "HeaderBar");
  gtk_window_set_titlebar(GTK_WINDOW(window), GTK_WIDGET(HeaderBar));

  /* We do not need the builder any more */
  g_object_unref (builder);
}
