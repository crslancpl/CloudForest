/*
 * now the gui parts of the app will not interact with core directly
 */
#include "guiCore.h"

#include "../Core.h"
#include "../Global.h"
#include "FilePanel.h"
#include "EditArea.h"
#include "MainWindow.h"
#include "HeaderBar.h"
#include "SettingPanel.h"
#include "Style.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>


MainWindow gui::AppWindow;
SettingPanel gui::AppSettingPanel;
HeaderBar gui::AppHeaderBar;
FilePanel gui::AppFilePanel;
EditAreaHolder* gui::FocusedEAHolder;

static std::vector<std::shared_ptr<EditArea>> EditAreas = {};


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

    /*
     * This will be moved to CFGrid
     */
    GtkGrid *grid = GTK_GRID(gtk_grid_new());
    EditAreaHolder *newholder = gui::NewEAHolder();
    GtkSeparator *separator = GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    gtk_grid_attach(grid, GTK_WIDGET(gui::AppFilePanel.BaseGrid), 0, 0, 1, 1);
    gtk_grid_attach(grid, GTK_WIDGET(separator), 1, 0, 1, 1);
    gtk_grid_attach(grid, GTK_WIDGET(newholder->BaseGrid), 2, 0, 1, 1);
    gtk_window_set_child(gui::AppWindow.Window, GTK_WIDGET(grid));
    gtk_widget_add_css_class(GTK_WIDGET(separator), "Separator");
    gtk_widget_set_size_request(GTK_WIDGET(separator), 5, 0);// height will be expanded
    GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator
    gtk_widget_add_controller(GTK_WIDGET(separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
    g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragged), nullptr);

    gui::FocusedEAHolder = newholder;
    std::shared_ptr<EditArea> *blankeditarea = gui::NewEditArea(nullptr);
    FocusedEAHolder->Show(*blankeditarea);
    gui::AppWindow.Show();
}



static std::vector<std::unique_ptr<EditAreaHolder>> EAHolders = {};

//static
EditAreaHolder* gui::GetEAHolder(int number){
    return EAHolders[number].get();
}
//static
EditAreaHolder*gui::NewEAHolder(){
    EAHolders.emplace_back(std::make_unique<EditAreaHolder>());
    EditAreaHolder* newholder = EAHolders.back().get();
    newholder->Init();

    return newholder;
}

std::shared_ptr<EditArea>* gui::NewEditArea(GFile *file){
    EditAreas.emplace_back(std::make_shared<EditArea>(file));
    return &EditAreas.back();
}

std::shared_ptr<EditArea>* gui::GetEditArea(const std::string &filepath){
    /*
     * Return nullptr if not found
     */
    for (std::shared_ptr<EditArea>& ea: EditAreas) {
        if(ea->AbsoPath == filepath){
            return &ea;
        }
    }
    return nullptr;
}


const results::Results* gui::Process(GUIAction *action){
    if(action->actiontype == GUIAction::GETEDITAREACONTENT){
        return GetEditAreaContent(action->filename);
    }else if(action->actiontype == GUIAction::DRAWBYPOS){
        auto ea = gui::GetEditArea(action->filename);
        (*ea)->ApplyTagByPos(action->startpos, action->endpos, (char*)action->otherdata);
        return nullptr;
    }else if(action->actiontype == GUIAction::DRAWBYLINE){
        auto ea = gui::GetEditArea(action->filename);
        (*ea)->ApplyTagByLinePos(action->line, action->offset, action->length, (char*)action->otherdata);
    }else if(action->actiontype == GUIAction::ADDTEXTCHANGEDCALLBACK){
        auto ea = gui::GetEditArea(action->filename);
        (*ea)->AddTextChangedPyCalback(action->othertext);
    }
    return nullptr;
}

//async
const results::Results* gui::GetEditAreaContent(const std::string &filepath){
    static results::GetText Result;
    auto ea = gui::GetEditArea(filepath);
    if(ea != nullptr){
        Result.text = &(*ea)->GetContent();
        return &Result;
    }else{
        return nullptr;
    }
}

//async
const results::Results* gui::GetOpenedEditArea(const std::string &filepath){
    static results::GetAllEditAreaPath eapaths;
    /*not finished */
    return nullptr;
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




void gui::PyRunCode(std::string &code){
    static PyAction action;
    action.Destination = Parts::PYTHON;
    action.code = code;
    core::Interact(&action);
}
