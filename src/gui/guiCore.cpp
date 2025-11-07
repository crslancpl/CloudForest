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

#include "../requests/GUIRequests.h"
#include "../requests/FileRequests.h"
#include "../requests/CFRequests.h"
#include "../requests/PyRequests.h"

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

    gui::AppWindow.Layout.InsertChild(GTK_WIDGET(gui::AppFilePanel.BaseGrid));
    gui::AppWindow.Layout.InsertChild(GTK_WIDGET(newholder->m_BaseGrid));

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
        std::string function = func + "(\"" + (*ea)->m_AbsoPath + "\")";
        gui::RunPythonCode(function);
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
        if(ea->m_AbsoPath == filepath){
            return &ea;
        }
    }
    return nullptr;
}


const result::Result* gui::Process(Request* request){
    EditArea* ea;
    if(auto req = dynamic_cast<EAGetText*>(request)){
        if(req->Parent != nullptr){
            ea = req->Parent;
        }else{
            ea = GetEditArea(req->Filepath)->get();
        }
        return GetEditAreaContent(ea);
    }else if(auto req = dynamic_cast<EADrawByPos*>(request)){
        req->Parent->ApplyTagByPos(req->Startpos, req->Endpos, (char*)req->Tagname.c_str());
    }else if(auto req = dynamic_cast<EADrawByLine*>(request)){
        if(req->Parent != nullptr){
            ea = req->Parent;
        }else{
            ea = GetEditArea(req->Filepath)->get();
        }
        ea->ApplyTagByLinePos(req->Line, req->Offset, req->Length, (char*)req->Tagname.c_str());
    }else if(auto req = dynamic_cast<EAClearSuggestion*>(request)){
        req->Parent->ClearSuggestion();
    }else if(auto req = dynamic_cast<EAAddSuggestion*>(request)){
        req->Parent->AddSuggestion(req->Sug);
    }else if(auto req = dynamic_cast<EAHideSuggestion*>(request)){
        req->Parent->HideSuggestion();
    }else if(auto req = dynamic_cast<EAShowSuggestion*>(request)){
        req->Parent->ShowSuggestion();
    }else if(auto req = dynamic_cast<EASetLang*>(request)){
        req->Parent->ChangeLanguage(req->Lang, false);
    }
    return nullptr;
}

//async
const result::Result* gui::GetEditAreaContent(EditArea *ea){
    static result::GetText Result;
    if(ea != nullptr){
        Result.Text = ea->GetContent().c_str();
        return &Result;
    }else{
        return nullptr;
    }
}

//async
const result::Result* gui::ChangeEditAreaLanguage(EditArea *ea, const std::string &lang){
    if(ea != nullptr){
        ea->ChangeLanguage(lang, true);
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
        static FileOpenFile req;
        req.Callback = FileChoosen;
        core::Interact(&req);
    }else{
        //folder
        static FileOpenFolder req;
        req.Callback = FileChoosen;
        core::Interact(&req);
    }
}

//async
void gui::SaveFile(GFile* file, char* content, void(*callback)(GFile*,GFileInfo*)){
    static FileSave req;
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
    static FileEnumerate req;
    req.File = folder;
    req.Callback = callback;

    core::Interact(&req);
}


void gui::cfLoadLanguage(const std::string& langname){
    /*
     * Tell CloudyForest to load the language template
     */
    static CFLoadTemplate req;
    req.Language = langname;

    core::Interact(&req);
}

void gui::cfProcessFile(const std::string& filepath, const std::string& language){
    /*
     * Tell CloudyForest to process a file. CloudyForest will
     * request for content later.
     */
    static CFReadFile req;
    req.Filepath = filepath;
    req.Language = language;

    core::Interact(&req);
}




void gui::RunPythonCode(std::string& code){
    static PyRunCode req;
    req.Code = code;
    core::Interact(&req);
}

void gui::PythonRegisterEA(EditArea *self){
    static PyRegisterEA req;
    req.ea = self;
    req.FilePath = self->m_AbsoPath;
    core::Interact(&req);
}

void gui::PythonCallbackEATextChanged(EditArea *self){
    static PyCallbackEA req;
    req.m_CallbackType = PyCallbackEA::TEXTCHANGED;
    req.ea = self;
    req.m_StartLine = self->m_CursorLine;
    req.m_StartPos = self->m_CursorLinePos;

    core::Interact(&req);
}

void gui::PythonCallbackEAReqCompletion(EditArea *self){
    static PyCallbackEA req;
    req.m_CallbackType = PyCallbackEA::REQUESTCOMPLETION;
    req.ea = self;
    req.m_StartLine = self->m_CursorLine;
    req.m_StartPos = self->m_CursorLinePos;

    core::Interact(&req);
}
