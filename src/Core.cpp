#include "Core.h"
#include "EditArea.h"
#include "FilePanel.h"
#include "MainWindow.h"
#include "PythonBackend/PythonMain.h"
#include "cf/CFEmbed.h"
#include "ToolFunctions.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>
#include <algorithm>
#include <node/node.h>

//Cf callback function
FileRequest *req;
//Message oMessage;// from here to cloudyforest (out)

void CfCallbackFunc(MessageType type, void *data){
    if(type == MessageType::FILEREQ){
        req = (FileRequest*)data;

        FileRespond *fresp = new FileRespond();
        fresp->FilePath = req->FilePath;

        if (StartWith(req->FilePath, "syntax")) {
            fresp->IsPath = true;
            fresp->Content = req->FilePath;
        }else{
            fresp->IsPath = false;
            EditArea *ea = GetEditAreaFromFileAbsoPath(req->FilePath)->get();
            if(ea == nullptr){
                g_print("null filename\n");
            }
            gtk_text_buffer_get_start_iter(ea->TextViewBuffer, ea->StartItr);
            gtk_text_buffer_get_end_iter(ea->TextViewBuffer, ea->EndItr);
            fresp->Content = gtk_text_buffer_get_text(ea->TextViewBuffer, ea->StartItr, ea->EndItr, true);
        }

        emb_Send_Message_To_CF(MessageType::FILERESP, fresp);
        //CfSendMessage(MessageType::FILERESP, fresp);

    }else if(type == MessageType::DRAW){
        Highlight *h = (Highlight*) data;
        EditArea *ea = GetEditAreaFromFileAbsoPath(h->file)->get();
        switch (h->type) {
        case CF_TYPE:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("type"));
        break;
        case CF_KEYWORD:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("keyword"));
        break;
        case CF_MULTCMT:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("cmt"));
        break;
        case CF_SINGCMT:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("scmt"));
        break;
        case CF_TEXT:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("text"));
        break;
        case CF_TAG:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("tag"));
        break;
        case CF_FUNCTIONNAME:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("func"));
        break;
        /* bugged
        case CF_NUM:
        ea->ApplyTagByLength(h->startpos, h->endpos, strdup("num"));
        break;*/
        default:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("none"));
        break;
        }
    }
}

void InitCfEmbed(){
    emb_Connect(CfCallbackFunc);
}

//App
GtkApplication *App;

void SetApp(GtkApplication* app){
    App = app;
    PyBackend::Start();
}

GtkApplication &GetApp(){
    return *App;
}

//Settings
void LoadSettings();

//Window
MainWindow AppWindow;

MainWindow &GetAppWindow(){
    return AppWindow;
}

// FilePanel
vector<shared_ptr<FPFolderButton>> AllFolderButtons;
vector<shared_ptr<FPFileButton>> AllFileButtons;

shared_ptr<FPFileButton>& NewFileButton(){
    /*
     * Create a file button in AllFileButton and return the reference
     */
    AllFileButtons.emplace_back(make_shared<FPFileButton>());
    return AllFileButtons[AllFileButtons.size() -1];
}

shared_ptr<FPFolderButton>& NewFolderButton(){
    /*
     * Create a folder button in AllFolderButton and return the reference
     */
    AllFolderButtons.emplace_back(make_shared<FPFolderButton>());
    return AllFolderButtons[AllFolderButtons.size() -1];// new Folder will by pushed to the last position
}


//EditArea
vector<shared_ptr<EditArea>> AllEditArea;

shared_ptr<EditArea>& NewEditArea(GFile* filetoedit, FPFileButton* filebutton){
    /*
     * Create a edit area in AllEditArea and return the reference
     */
    AllEditArea.emplace_back(make_shared<EditArea>(filetoedit, filebutton));
    return AllEditArea[AllEditArea.size()-1];
}

void RemoveEditArea(EditArea* EditAreaPtr){
    int Pos = 0;
    for(shared_ptr<EditArea> ea: AllEditArea){
        if(ea.get() == EditAreaPtr){
            gtk_widget_unparent(GTK_WIDGET(ea->BaseGrid));
            break;
        }
        Pos++;
    }
    AllEditArea.erase(AllEditArea.begin() + Pos);
}

shared_ptr<EditArea>* GetEditAreaFromFileAbsoPath(const string &AbsoPath){
    for(shared_ptr<EditArea>& ea: AllEditArea){
        if(ea->AbsoPath == AbsoPath){
            return &ea;
        }
    }
    return nullptr;
}

shared_ptr<EditArea>* GetEditAreaFromFileRelaPath(const string &RelaPath){
    for(shared_ptr<EditArea>& ea: AllEditArea){
        if(ea->RelePath == RelaPath){
            return &ea;
        }
    }
    return nullptr;
}
