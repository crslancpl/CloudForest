#include "Gui_if.h"

#include "editarea/EditArea_if.h"
#include "filepanel/FilePanel.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "style/Style.h"
#include "layouts/tab/CfTab_if.h"
#include "layouts/tab/CfTabLayout.h"

#include <gtk/gtk.h>

static MainWindow* current_main_window;
static FilePanel* current_file_panel;
static SettingPanel* current_setting_panel;


static void AppActivated (GtkApplication *app, gpointer user_data){
    style::LoadCssFolder("data/styles/DefaultDarkTheme");

    current_main_window = new MainWindow(app);// freed on app closed
    current_file_panel = new FilePanel();// freed on app closed
    current_setting_panel = new SettingPanel(current_main_window);// freed on app closed

    current_main_window->Insert(current_file_panel);
    current_main_window->Insert(tablayout::NewTabLayout());

    editarea::EditNewFile();
    current_main_window->Show();
}

static void AppClosed (GtkApplication *app, gpointer user_data){
    printf("\nGtk application closed\n");
}

namespace gui{

int RunApp(int argc, char* argv[]){
#ifdef GTK_SRCDIR
    g_chdir (GTK_SRCDIR);
#endif
    auto app =gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);
    g_signal_connect (app, "activate", G_CALLBACK (AppActivated), nullptr);
    g_signal_connect(app, "shutdown", G_CALLBACK(AppClosed), nullptr);

    int status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}

MainWindow* GetMainWindow(){
    return current_main_window;
}

FilePanel* GetFilePanel(){
    return current_file_panel;
}

SettingPanel* GetSettingPanel(){
    return current_setting_panel;
}


}// namespace gui
