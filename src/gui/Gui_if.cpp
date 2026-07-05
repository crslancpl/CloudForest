#include "Gui_if.h"

#include "AppUI.h"
#include "editarea/DiagnosticPanel.h"
#include "editarea/LangPanel.h"
#include "filepanel/FilePanel.h"
#include "headerbar/Headerbar.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "style/Style.h"
#include "src/App.h"
#include "src/session/EditAreaData.h"
#include "layouts/tab/CfTab_if.h"
#include "layouts/tab/CfTabLayout.h"

#include <gtk/gtk.h>

static App* current_app;

static void AppActivated (GtkApplication *app, gpointer user_data){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");
    session::InitEditAreaData();

    AppUI& appui = current_app->appUI;

    appui.gtkApp = app;
    /*
     * The custructor will set itself as a member of appui
     */
    HeaderBar* headerbar = new HeaderBar(appui);
    MainWindow* mainwindow = new MainWindow(appui);
    FilePanel* filepanel = new FilePanel(appui);
    SettingPanel* settingpanel = new SettingPanel(appui);
    LangPanel* langPanel = new LangPanel(appui);
    DiagnosticPanel* diagnosticPanel = new DiagnosticPanel(appui);


    mainwindow->Insert(filepanel);

    CfTabLayout* tab = new CfTabLayout();// freed on app closed
    mainwindow->Insert(tab);

    session::EditNewFile();
    mainwindow->Show();
}

static void AppClosed (GtkApplication *app, gpointer user_data){
    printf("\nGtk application closed\n");
}

namespace gui{

int RunApp(int argc, char* argv[], App& app){
#ifdef GTK_SRCDIR
    g_chdir (GTK_SRCDIR);
#endif
    current_app = &app;
    auto gtkapp =gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);
    g_signal_connect (gtkapp, "activate", G_CALLBACK (AppActivated), nullptr);
    g_signal_connect(gtkapp, "shutdown", G_CALLBACK(AppClosed), nullptr);

    int status = g_application_run (G_APPLICATION (gtkapp), argc, argv);
    g_object_unref (gtkapp);
    return status;
}

MainWindow* GetMainWindow(){
    return current_app->appUI.mainWindow;
}

FilePanel* GetFilePanel(){
    return current_app->appUI.filePanel;
}

SettingPanel* GetSettingPanel(){
    return current_app->appUI.settingPanel;
}

DiagnosticPanel* GetDiagnosticPanel(){
    return current_app->appUI.diagnosticPanel;
}

LangPanel* GetLangPanel(){
    return current_app->appUI.langPanel;
}

}// namespace gui
