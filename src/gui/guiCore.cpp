/*
 * now the gui parts of the app will not interact with core directly
 */
#include "guiCore.h"

#include "../Core.h"
#include "FilePanel.h"
#include "EditArea.h"
#include "MainWindow.h"
#include "HeaderBar.h"
#include "SettingPanel.h"
#include "Style.h"

#include <cstdlib>
#include <memory>
#include <vector>


MainWindow gui::AppWindow;
SettingPanel gui::AppSettingPanel;
HeaderBar gui::AppHeaderBar;
FilePanel gui::AppFilePanel;


void gui::Init(){
    style::LoadCssFromPath("styles/DefaultDarkTheme.css");
    gui::AppWindow.Init();
    gui::AppSettingPanel.Init();
    gui::AppHeaderBar.Init();
    gui::AppFilePanel.init();
    gui::AppWindow.SetHeaderBar(GTK_WIDGET(AppHeaderBar.HeaderBarWidget));
    GtkGrid *grid = GTK_GRID(gtk_grid_new());
    gtk_grid_attach(grid, GTK_WIDGET(gui::AppFilePanel.BaseGrid), 0, 0, 1, 1);
    gtk_window_set_child(gui::AppWindow.Window, GTK_WIDGET(grid));
    gui::AppWindow.Show();
}

void gui::OpenSettingPanel(){
    //
}

void gui::OpenLangChooser(EditArea *parent){
    //
}

FileAction action;
void gui::OpenFileChooser(bool fileordir, void (*callback)(GFile*,GFileInfo*)){
    action.Destination = Parts::FILEMANAGER;
    action.callback = callback;
    if(fileordir){
        //file
        action.actiontype = FileAction::OPENFILE;
    }else{
        //folder
        action.actiontype = FileAction::OPENFOLDER;
    }

    core::Interact(&action);
}

void gui::EnumFolder(GFile *folder, void (*callback)(GFile*,GFileInfo*)){
    action.Destination = Parts::FILEMANAGER;
    action.callback = callback;
    action.actiontype = FileAction::ENUMERATE;
    action.data = folder;
    core::Interact(&action);
}
