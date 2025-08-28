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
#include "FilePanel.h"
#include "EditArea.h"
#include "SettingPanel.h"
#include "Core.h"

#include <memory>
#include <type_traits>



void NewWindow (GtkApplication *app, gpointer user_data){
    /*
     * NewWindow funtion does not create a new MainWindow object. Instead, it retrieve the
     * AppWindow (MainWindow object) from Core.h and bind the properties to it from ui file
     * UI/MainWindow.ui. This function will set the current GtkApp and initialize other crucial
     * Items of the app.
     */

    SetApp(app);
    GtkBuilder *builder = gtk_builder_new ();
    InitCfEmbed();

    /* Build window by builder */
    gtk_builder_add_from_file (builder, "UI/MainWindow.ui", nullptr);
    MainWindow &AppWindow = GetAppWindow();

    /* Get the AppWindow class and assign variables */
    AppWindow.Window = GTK_WINDOW(gtk_builder_get_object (builder, "MainWindow"));
    AppWindow.WindowGrid = GTK_GRID(gtk_builder_get_object(builder,"WindowGrid"));
    gtk_window_set_default_size(AppWindow.Window, 1200, 800);
    gtk_window_set_application (AppWindow.Window, app);

    // Top: header bar
    AppWindow.Headerbar = LoadHeaderBar(builder);
    gtk_window_set_titlebar(GTK_WINDOW(AppWindow.Window), GTK_WIDGET(AppWindow.Headerbar->HeaderBarWidget));

    // Left: file panel
    FilePanel::OffSet=20;
    AppWindow.FP=new FilePanel();
    AppWindow.FP->init();
    gtk_grid_attach(AppWindow.WindowGrid,GTK_WIDGET(AppWindow.FP->BaseGrid), 0, 0, 1,1 );

    // Middle: separator that can be drag to resize file panel
    AppWindow.Separator = GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    gtk_widget_add_css_class(GTK_WIDGET(AppWindow.Separator), "Separator");
    gtk_widget_set_size_request(GTK_WIDGET(AppWindow.Separator), 5, 0);// height will be expanded
    GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator
    gtk_widget_add_controller(GTK_WIDGET(AppWindow.Separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
    g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragUpdate), nullptr);
    gtk_grid_attach(AppWindow.WindowGrid, GTK_WIDGET(AppWindow.Separator), 1, 0, 1, 1);

    // Right: edit area holder
    AppWindow.EAHolder = new EditAreaHolder();
    AppWindow.EAHolder->Init();
    gtk_grid_attach(AppWindow.WindowGrid, GTK_WIDGET(AppWindow.EAHolder->BaseGrid), 2, 0, 1, 1);

    /* Creates a EditArea that doesn't open any file */
    shared_ptr<EditArea>& NewEa = NewEditArea(nullptr, nullptr);
    AppWindow.EAHolder->Show(NewEa);

    LoadCssFromPath("styles/DefaultDarkTheme.css");
    InitFileManager(&AppWindow);
    InitSettingPanel();
    gtk_widget_set_visible (GTK_WIDGET(AppWindow.Window), TRUE);

    AppWindow.FP->UnrefBuilder();
    g_object_unref (builder);
}

void SeparatorDragUpdate(GtkGestureDrag* self, gdouble x, gdouble y, gpointer user_data){
    gtk_widget_set_size_request(GTK_WIDGET(GetAppWindow().FP->BaseGrid),
        gtk_widget_get_width(GTK_WIDGET(GetAppWindow().FP->BaseGrid)) + x, 0);
}
