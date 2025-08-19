#include "Core.h"
#include "EditArea.h"
#include "FilePanel.h"
#include "MainWindow.h"
#include "cf/CFEmbed.h"

#include <cstring>
#include <memory>
#include <vector>
#include <algorithm>


//Cf callback function
FileRequest *req;
Message oMessage;// from here to cloudyforest (out)

void CfCallbackFunc(Message *iMessage){
    if(iMessage->Type == MessageType::FILEREQ){
        req = (FileRequest*)iMessage->Data;

        FileRespond *fresp = new FileRespond();
        fresp->FilePath = req->FilePath;

        if (strcmp(req->FilePath, "LangTemp/lang/Template.txt") == 0) {
            fresp->IsPath = true;
            fresp->Content = "syntaxhighlight/cpp.txt";
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

        CfSendMessage(MessageType::FILERESP, fresp);

    }else if(iMessage->Type == MessageType::DRAW){
        Highlight *h = (Highlight*) iMessage->Data;
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
        default:
        ea->ApplyTagByPos(h->startpos, h->endpos, strdup("none"));
        break;
        }
    }
}

void InitCfEmbed(){
    emb_Connect(CfCallbackFunc);
}

void CfSendMessage(MessageType type, void* content){
    oMessage.Data = content;
    oMessage.Type = type;
    emb_Send_Message_To_CF(&oMessage);
}

//App
GtkApplication *App;

void SetApp(GtkApplication* app){
    App = app;
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

shared_ptr<EditArea>* GetEditAreaFromFileRelePath(const string &RelePath){
    for(shared_ptr<EditArea>& ea: AllEditArea){
        if(ea->RelePath == RelePath){
            return &ea;
        }
    }
    return nullptr;
}
