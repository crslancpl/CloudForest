#include "Gui_if.h"

#include "AppUI.h"
#include "editarea/CompletionPopover.h"
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

#include "editarea/CompletionTool.h"

#include <cstdio>
#include <gtk/gtk.h>
#include <memory>

static App* current_app;

static void AppActivated (GtkApplication *gtkapp, App* app){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");
    session::InitEditAreaData();

    AppUI& appui = current_app->appUI;

    appui.headerBar = std::make_unique<HeaderBar>(appui);
    appui.mainWindow = std::make_unique<MainWindow>(appui);
    appui.filePanel = std::make_unique<FilePanel>(appui);
    appui.settingPanel = std::make_unique<SettingPanel>(appui);
    appui.langPanel = std::make_unique<LangPanel>(appui);
    appui.diagnosticPanel = std::make_unique<DiagnosticPanel>(appui);
    //appui.completionPopover = std::make_unique<CompletionPopover>();

    appui.mainWindow->SetHeaderBar(*appui.headerBar);
    appui.mainWindow->Insert(*appui.filePanel);
    CfTabLayout* tab = new CfTabLayout();// freed on app closed
    appui.mainWindow->Insert(*tab);

    session::EditNewFile();
    app->appUI.mainWindow->Show();
}

static void AppClosed (GtkApplication *gtkapp, App* app){
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
    return current_app->appUI.mainWindow.get();
}

FilePanel* GetFilePanel(){
    return current_app->appUI.filePanel.get();
}

SettingPanel* GetSettingPanel(){
    return current_app->appUI.settingPanel.get();
}

DiagnosticPanel* GetDiagnosticPanel(){
    return current_app->appUI.diagnosticPanel.get();
}

LangPanel* GetLangPanel(){
    return current_app->appUI.langPanel.get();
}


void TransferCompletionPopover(CompletionTool* newowner){
    static CompletionTool* owner = nullptr;
    std::unique_ptr<CompletionPopover> popover;
    if (owner) {
        popover = owner->GetPopoverOwnership();
    } else if (!popover) {
        popover = std::make_unique<CompletionPopover>();
    }

    if (newowner) {
        newowner->SetPopoverOwnership(std::move(popover));
        owner = newowner;
    }
}

}// namespace gui
