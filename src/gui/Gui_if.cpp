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

static void AppActivated (GtkApplication *gtkapp, App* app){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");
    session::InitEditAreaData();

    AppUI& appui = current_app->appUI;

    appui.headerBar = new HeaderBar(appui);
    appui.mainWindow = new MainWindow(appui);
    appui.filePanel = new FilePanel(appui);
    appui.settingPanel = new SettingPanel(appui);
    appui.langPanel = new LangPanel(appui);
    appui.diagnosticPanel = new DiagnosticPanel(appui);

    appui.mainWindow->SetHeaderBar(*appui.headerBar);
    appui.mainWindow->Insert(*appui.filePanel);
    CfTabLayout* tab = new CfTabLayout();// freed on app closed
    appui.mainWindow->Insert(*tab);

    session::EditNewFile();
    app->appUI.mainWindow->Show();
}

static void AppClosed (GtkApplication *gtkapp, App* app){
    AppUI& appui = current_app->appUI;

    delete appui.headerBar;
    delete appui.mainWindow;
    delete appui.filePanel;
    delete appui.settingPanel;
    delete appui.langPanel;
    delete appui.diagnosticPanel;
    printf("\nGtk application closed\n");
}

namespace gui{

int RunApp(int argc, char* argv[], App& app){
#ifdef GTK_SRCDIR
    g_chdir (GTK_SRCDIR);
#endif
    current_app = &app;
    AppUI& appui = current_app->appUI;

    appui.gtkApp = gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);

    g_signal_connect(appui.gtkApp, "activate", G_CALLBACK (AppActivated), &app);
    g_signal_connect(appui.gtkApp, "shutdown", G_CALLBACK(AppClosed), &app);

    int status = g_application_run (G_APPLICATION (appui.gtkApp), argc, argv);// loop
    g_object_unref (appui.gtkApp);

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
