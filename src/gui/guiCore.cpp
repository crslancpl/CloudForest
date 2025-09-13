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
#include <glib/gprintf.h>
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
    style::InitLangChooser();

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

EditAreaHolder* gui::GetEAHolder(int number){
    return EAHolders[number].get();
}

EditAreaHolder* gui::NewEAHolder(){
    EAHolders.emplace_back(std::make_unique<EditAreaHolder>());
    EditAreaHolder* newholder = EAHolders.back().get();
    newholder->Init();
    return newholder;
}

static std::vector<std::string> EditAreaCreatedCallback = {};

std::shared_ptr<EditArea>* gui::NewEditArea(GFile *file){
    EditAreas.emplace_back(std::make_shared<EditArea>(file));
    std::shared_ptr<EditArea>* ea = &EditAreas.back();
    for (std::string& func : EditAreaCreatedCallback) {
        std::string function = func + "(\"" + (*ea)->AbsoPath + "\")";
        gui::PyRunCode(function);
    }
    return ea;
}

void gui::RemoveEditArea(EditArea* editarea){
    unsigned int pos = 0;

    editarea->Destroy();

    for (std::shared_ptr<EditArea>& ea : EditAreas) {
        if(ea.get() == editarea){
            EditAreas.erase(EditAreas.begin() + pos);
            return;
        }
        pos++;
    }
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


const result::Result* gui::Process(request::Request* request){
    if(auto req = dynamic_cast<request::EAGetText*>(request)){
        return GetEditAreaContent(req->Filepath);
    }else if(auto req = dynamic_cast<request::EADrawByPos*>(request)){
        auto ea = gui::GetEditArea(req->Filepath);
        (*ea)->ApplyTagByPos(req->Startpos, req->Endpos, (char*)req->Tagname.c_str());
    }else if(auto req = dynamic_cast<request::EADrawByLine*>(request)){
        auto ea = gui::GetEditArea(req->Filepath);
        (*ea)->ApplyTagByLinePos(req->Line, req->Offset, req->Length, (char*)req->Tagname.c_str());
    }else if(auto req = dynamic_cast<request::EAAddCallBack*>(request)){
        auto ea = gui::GetEditArea(req->Filepath);
        switch (req->Type) {
        case request::EAAddCallBack::NEWEDITAREA:
            EditAreaCreatedCallback.emplace_back(req->Funcname);
            break;
        case request::EAAddCallBack::TEXTCHANGED:
            (*ea)->AddTextChangedPyCalback(req->Funcname);
            break;
        default:
            break;
        }
    }
    return nullptr;
}

//async
const result::Result* gui::GetEditAreaContent(const std::string &filepath){
    static result::GetText Result;
    auto ea = gui::GetEditArea(filepath);
    if(ea != nullptr){
        Result.Text = &(*ea)->GetContent();
        return &Result;
    }else{
        return nullptr;
    }
}

//async
const result::Result* gui::GetOpenedEditArea(const std::string &filepath){
    static result::GetAllEditAreaPath eapaths;
    /*not finished */
    return nullptr;
}



//async
void gui::OpenFileChooser(bool fileordir, void (*callback)(GFile*,GFileInfo*)){
    /*
     * Tell file manager to open file chooser
     */

    if(fileordir){
        //file
        static request::FileOpenFile req;
        req.Callback = callback;
        core::Interact(&req);
    }else{
        //folder
        static request::FileOpenFolder req;
        req.Callback = callback;
        core::Interact(&req);
    }


}

//async
void gui::SaveFile(GFile* file, char* content, void(*callback)(GFile*,GFileInfo*)){
    static request::FileSave req;
    req.Content = content;
    req.File = file;
    req.Callback = callback;
    core::Interact(&req);
}

//async
void gui::EnumFolder(GFile *folder, void (*callback)(GFile*,GFileInfo*)){
    /*
     * Tell file manager to enumerate folder
     */
    static request::FileEnumerate req;
    req.File = folder;
    req.Callback = callback;

    core::Interact(&req);
}


void gui::cfLoadLanguage(const std::string& langname){
    /*
     * Tell CloudyForest to load the language template
     */
    static request::CFLoadTemplate req;
    req.Language = langname;

    core::Interact(&req);
}

void gui::cfProcessFile(const std::string& filepath, const std::string& language){
    /*
     * Tell CloudyForest to process a file. CloudyForest will
     * request for content later.
     */
    static request::CFReadFile req;
    req.Filepath = filepath;
    req.Language = language;

    core::Interact(&req);
}




void gui::PyRunCode(std::string& code){
    static request::PyRunCode req;
    req.Code = code;
    core::Interact(&req);
}
