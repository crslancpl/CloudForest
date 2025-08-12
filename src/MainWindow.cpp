#include <cstddef>
#include <cstdlib>
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <gtk/gtkcssprovider.h>
#include <gtk/gtkshortcut.h>
#include <gtk/gtkstyleprovider.h>
#include <glib/gfileutils.h>


#include "MainWindow.h"

#include "Classes.h"
#include "CssLoader.h"
#include "FileManager.h"
#include "HeaderBar.h"
#include <memory>
#include <type_traits>

#include "FilePanel.h"
#include "EditArea.h"
#include "Core.h"


void NewWindow (GtkApplication *app, gpointer user_data){
    GtkBuilder *builder = gtk_builder_new ();
    /* Constructing MainWindow */
    gtk_builder_add_from_file (builder, "UI/MainWindow.ui", nullptr);
    MainWindow &AppWindow = GetAppWindow();

    AppWindow.Window = GTK_WINDOW(gtk_builder_get_object (builder, "MainWindow"));
    AppWindow.WindowGrid = GTK_GRID(gtk_builder_get_object(builder,"WindowGrid"));
    AppWindow.App = app;
    AppWindow.FP=new FilePanel();
    AppWindow.FP->init();
    AppWindow.EAHolder = new EditAreaHolder();
    AppWindow.EAHolder->Init();

    FilePanel::OffSet=20;

    gtk_window_set_default_size(AppWindow.Window, 1200, 800);
    gtk_window_set_application (AppWindow.Window, app);
    shared_ptr<EditArea>& NewEa = NewEditArea(nullptr, nullptr);
    AppWindow.EAHolder->Show(NewEa);

    LoadCssFromPath("styles/DefaultDarkTheme.css");

    // share a builder so it can be unref later at once

    gtk_widget_set_visible (GTK_WIDGET(AppWindow.Window), TRUE);

    /* Load Headerbar */
    HeaderBar *hb = LoadHeaderBar(builder, AppWindow.App);
    gtk_window_set_titlebar(GTK_WINDOW(AppWindow.Window), GTK_WIDGET(hb->HeaderBar));

    InitFileManager(&AppWindow);

    gtk_grid_attach(AppWindow.WindowGrid,GTK_WIDGET(AppWindow.FP->BaseGrid), 0, 0, 1,1 );
    gtk_grid_attach(AppWindow.WindowGrid, GTK_WIDGET(AppWindow.EAHolder->BaseGrid), 1, 0, 1, 1);

    g_object_unref (builder);
}
