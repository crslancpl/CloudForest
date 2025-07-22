#include <cstddef>
#include <cstdlib>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <gtk/gtkcssprovider.h>
#include <gtk/gtkshortcut.h>
#include <gtk/gtkstyleprovider.h>
#include <glib/gfileutils.h>


#include "MainWindow.h"

#include "DataTypes.h"
#include "CssLoader.h"
#include "FileManager.h"
#include "HeaderBar.h"
#include <memory>

MainWindow ThisWindow;

void NewWindow (GtkApplication *app, gpointer user_data){

    ThisWindow = MainWindow();
    ThisWindow.FP=(FilePanel *)malloc(sizeof(FilePanel));
    ThisWindow.FP->init();
    ThisWindow.EAHolder = (EditAreaHolder*)malloc(sizeof(EditAreaHolder));
    ThisWindow.EAHolder->Init();
    /* Construct a GtkBuilder instance and load our UI description */
    GtkBuilder *builder = gtk_builder_new ();
    /* Constructing MainWindow */
    gtk_builder_add_from_file (builder, "UI/MainWindow.ui", NULL);
    ThisWindow.Window = GTK_WINDOW(gtk_builder_get_object (builder, "MainWindow"));
    ThisWindow.WindowGrid = GTK_GRID(gtk_builder_get_object(builder,"WindowGrid"));
    ThisWindow.App = app;
    gtk_window_set_default_size(ThisWindow.Window, 1200, 800);
    gtk_window_set_application (ThisWindow.Window, app);

    LoadCssFromPath("UI/FilePanel.css");
    LoadCssFromPath("UI/MainWindow.css");
    LoadCssFromPath("UI/HeaderBar.css");
    LoadCssFromPath("UI/EditArea.css");

    // share a builder so it can be unref later at once

    gtk_widget_set_visible (GTK_WIDGET(ThisWindow.Window), TRUE);

    /* Load Headerbar */
    HeaderBar *hb = LoadHeaderBar(builder, ThisWindow.App);
    gtk_window_set_titlebar(GTK_WINDOW(ThisWindow.Window), GTK_WIDGET(hb->HeaderBar));

    InitFileManager(&ThisWindow);

    gtk_grid_attach(ThisWindow.WindowGrid,GTK_WIDGET(ThisWindow.FP->BaseGrid), 0, 0, 1,1 );
    gtk_grid_attach(ThisWindow.WindowGrid, GTK_WIDGET(ThisWindow.EAHolder->BaseGrid), 1, 0, 1, 1);

    /* We do not need the builder any more */
    g_object_unref (builder);
}
