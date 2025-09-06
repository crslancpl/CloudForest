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


static void SeparatorDragged(GtkGestureDrag* self, gdouble x, gdouble y, gpointer d){
    gtk_widget_set_size_request(GTK_WIDGET(gui::AppFilePanel.BaseGrid),
        gtk_widget_get_width(GTK_WIDGET(gui::AppFilePanel.BaseGrid)) + x, 0);
}

void gui::Init(){
    style::LoadCssFromPath("styles/DefaultDarkTheme.css");
    gui::AppWindow.Init();
    gui::AppSettingPanel.Init();
    gui::AppHeaderBar.Init();
    gui::AppFilePanel.init();
    gui::AppWindow.SetHeaderBar(GTK_WIDGET(AppHeaderBar.HeaderBarWidget));
    GtkGrid *grid = GTK_GRID(gtk_grid_new());
    EditAreaHolder *newholder = EditAreaHolder::New();
    GtkSeparator *separator = GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    gtk_grid_attach(grid, GTK_WIDGET(gui::AppFilePanel.BaseGrid), 0, 0, 1, 1);
    gtk_grid_attach(grid, GTK_WIDGET(separator), 1, 0, 1, 1);
    gtk_grid_attach(grid, GTK_WIDGET(newholder->BaseGrid), 2, 0, 1, 1);
    gtk_window_set_child(gui::AppWindow.Window, GTK_WIDGET(grid));
    gui::AppWindow.Show();

    gtk_widget_add_css_class(GTK_WIDGET(separator), "Separator");
    gtk_widget_set_size_request(GTK_WIDGET(separator), 5, 0);// height will be expanded
    GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator
    gtk_widget_add_controller(GTK_WIDGET(separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
    g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragged), nullptr);
}

void gui::Process(GUIAction *action){
    if(action->actiontype == GUIAction::GETEDITAREACONTENT){
        GetEditAreaContent(action->filename, action->callback);
    }else if(action->actiontype ==GUIAction::DRAW){
        auto ea = EditArea::Get(action->filename);
        (*ea)->ApplyTagByPos(action->startpos, action->endpos, (char*)action->otherdata);
    }
}

static FileAction fileaction;
//async
void gui::OpenFileChooser(bool fileordir, void (*callback)(GFile*,GFileInfo*)){
    /*
     * Tell file manager to open file chooser
     */
    fileaction.Destination = Parts::FILEMANAGER;
    fileaction.callback = callback;
    if(fileordir){
        //file
        fileaction.actiontype = FileAction::OPENFILE;
    }else{
        //folder
        fileaction.actiontype = FileAction::OPENFOLDER;
    }

    core::Interact(&fileaction);
}

//async
void gui::SaveFile(GFile* file, char* content, void(*callback)(GFile*,GFileInfo*)){
    fileaction.Destination = Parts::FILEMANAGER;
    fileaction.actiontype = FileAction::SAVE;
    fileaction.data = content;
    fileaction.file = file;
    fileaction.callback = callback;
    core::Interact(&fileaction);
}

//async
void gui::EnumFolder(GFile *folder, void (*callback)(GFile*,GFileInfo*)){
    /*
     * Tell file manager to enumerate folder
     */
    fileaction.Destination = Parts::FILEMANAGER;
    fileaction.callback = callback;
    fileaction.actiontype = FileAction::ENUMERATE;
    fileaction.data = folder;
    core::Interact(&fileaction);
}


static CFAction cfaction;
void gui::cfLoadLanguage(const std::string& langname){
    /*
     * Tell CloudyForest to load the language template
     */
    cfaction.Destination = Parts::CLOUDYFOREST;
    cfaction.actiontype = CFAction::LOADTEMP;
    cfaction.language = langname;
    core::Interact(&cfaction);
}

void gui::cfProcessFile(const std::string& filepath, const std::string& language){
    /*
     * Tell CloudyForest to process a file. CloudyForest will
     * request for content later.
     */
    cfaction.Destination = Parts::CLOUDYFOREST;
    cfaction.actiontype = CFAction::READFILE;
    cfaction.language = language;
    cfaction.filepath = filepath;
    core::Interact(&cfaction);
}

//async
void gui::GetEditAreaContent(const std::string &filepath, void (*callback)(char*)){
    auto ea = EditArea::Get(filepath);
    if(ea != nullptr){
        callback((*ea)->GetContent());
    }
}
