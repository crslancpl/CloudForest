/*
 * now the gui parts of the app will not interact with core directly
 */
#include "guiCore.h"

#include "../Core.h"
#include "../Global.h"
#include "FilePanel.h"
#include "CFLayout.h"
#include "EditArea.h"
#include "MainWindow.h"
#include "HeaderBar.h"
#include "SettingPanel.h"
#include "Style.h"

#include <cstdlib>
#include <glib/gprintf.h>
#include <gtk/gtklayoutmanager.h>
#include <memory>
#include <string>
#include <vector>


MainWindow gui::AppWindow;
SettingPanel gui::AppSettingPanel;
HeaderBar gui::AppHeaderBar;
FilePanel gui::AppFilePanel;
EditAreaHolder* gui::FocusedEAHolder;

static std::vector<std::shared_ptr<EditArea>> EditAreas = {};

void gui::Init(){
    style::LoadCssFromPath("styles/DefaultDarkTheme.css");
    gui::AppWindow.Init();
    gui::AppSettingPanel.Init();
    gui::AppHeaderBar.Init();
    gui::AppFilePanel.init();
    gui::AppWindow.SetHeaderBar(GTK_WIDGET(AppHeaderBar.HeaderBarWidget));
    style::InitLangChooser();

    EditAreaHolder *newholder = gui::NewEAHolder();
    gui::FocusedEAHolder = newholder;
    gui::FocusedEAHolder->Show(*gui::NewEditArea(nullptr));

    gui::AppWindow.Layout->InsertChild(GTK_WIDGET(gui::AppFilePanel.BaseGrid));
    gui::AppWindow.Layout->InsertChild(GTK_WIDGET(newholder->BaseGrid));

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
const result::Result* gui::ChangeEditAreaLanguage(const std::string &filepath, const std::string &lang){
    auto ea = gui::GetEditArea(filepath);
    if(ea != nullptr){
        (*ea)->ChangeLanguage(lang, true);
    }
    /*not finished */
    return nullptr;
}



static void FileChoosen(GFile *file, GFileInfo *fileinfo){
    gui::AppFilePanel.LoadRoot(file, fileinfo);
}

//async
void gui::OpenFileChooser(bool fileordir){
    /*
     * Tell file manager to open file chooser
     */

    if(fileordir){
        //file
        static request::FileOpenFile req;
        req.Callback = FileChoosen;
        core::Interact(&req);
    }else{
        //folder
        static request::FileOpenFolder req;
        req.Callback = FileChoosen;
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
