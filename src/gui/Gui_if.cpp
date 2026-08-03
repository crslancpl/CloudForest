#include "Gui_if.h"

#include "AppUI.h"
#include "editarea/DiagnosticPanel.h"
#include "filepanel/FilePanel.h"
#include "headerbar/Headerbar.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "style/Style.h"
#include "src/App.h"
#include "src/session/EditAreaData.h"

#include <cstdio>
#include <gtk/gtk.h>

static App* current_app;

static void AppActivated (GtkApplication *gtkapp, App* app){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");
    session::InitEditAreaData();

    AppUI& appui = current_app->appUI;
    appui.Start(gtkapp);

    session::EditNewFile();
    appui.GetMainWindow()->Show();
}

static void AppClosed (GtkApplication *gtkapp, App* app){
    app->appUI.Finalize();
    printf("\nGtk application closed\n");
}

namespace gui{

AppUI& GetCurrentUI(){
    return current_app->appUI;
}

int RunApp(int argc, char* argv[], App& app){
#ifdef GTK_SRCDIR
    g_chdir (GTK_SRCDIR);
#endif
    current_app = &app;
    AppUI& appui = current_app->appUI;
    GtkApplication* gtkapp = gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);

    g_signal_connect(gtkapp, "activate", G_CALLBACK (AppActivated), &app);
    g_signal_connect(gtkapp, "shutdown", G_CALLBACK(AppClosed), &app);
    int status = g_application_run (G_APPLICATION (gtkapp), argc, argv);// loop
    g_object_unref (gtkapp);

    return status;
}

}// namespace gui
