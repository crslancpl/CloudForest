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



void NewWindow (GtkApplication *app, gpointer user_data){
    FilePanel::OffSet=20;
    SectionData::currentwindow = MainWindow();
    SectionData::currentwindow.FP=new FilePanel();
    SectionData::currentwindow.FP->init();
    SectionData::currentwindow.EAHolder = new EditAreaHolder();
    SectionData::currentwindow.EAHolder->Init();
    /* Construct a GtkBuilder instance and load our UI description */
    GtkBuilder *builder = gtk_builder_new ();
    /* Constructing MainWindow */
    gtk_builder_add_from_file (builder, "UI/MainWindow.ui", NULL);
    SectionData::currentwindow.Window = GTK_WINDOW(gtk_builder_get_object (builder, "MainWindow"));
    SectionData::currentwindow.WindowGrid = GTK_GRID(gtk_builder_get_object(builder,"WindowGrid"));
    SectionData::currentwindow.App = app;
    gtk_window_set_default_size(SectionData::currentwindow.Window, 1200, 800);
    gtk_window_set_application (SectionData::currentwindow.Window, app);
    shared_ptr<EditArea> NewEa = make_shared<EditArea>((GFile*)NULL);
    SectionData::AddEditArea(NewEa);
    SectionData::currentwindow.EAHolder->Show(NewEa);

    LoadCssFromPath("styles/DefaultTheme.css");



    // share a builder so it can be unref later at once

    gtk_widget_set_visible (GTK_WIDGET(SectionData::currentwindow.Window), TRUE);

    /* Load Headerbar */
    HeaderBar *hb = LoadHeaderBar(builder, SectionData::currentwindow.App);
    gtk_window_set_titlebar(GTK_WINDOW(SectionData::currentwindow.Window), GTK_WIDGET(hb->HeaderBar));

    InitFileManager(&SectionData::currentwindow);

    gtk_grid_attach(SectionData::currentwindow.WindowGrid,GTK_WIDGET(SectionData::currentwindow.FP->BaseGrid), 0, 0, 1,1 );
    gtk_grid_attach(SectionData::currentwindow.WindowGrid, GTK_WIDGET(SectionData::currentwindow.EAHolder->BaseGrid), 1, 0, 1, 1);

    /* We do not need the builder any more */
    g_object_unref (builder);
}
