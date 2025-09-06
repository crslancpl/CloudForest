#include "CFCore.h"

#include <cstring>
#include <gtk/gtk.h>
#include "../Core.h"
#include "../ToolFunctions.h"
#include "CFEmbed.h"

static FileRespond resp;
static Entry ent;
static GUIAction guiaction;

static void GotContentFromEditArea(char* content){
    resp.IsPath = false;
    resp.Content = content;
    //g_print("%s\n", content);
    emb_Send_Message_To_CF(FILERESP, &resp);
}

static void RequestFile(FileRequest *req){
    resp.FilePath = req->FilePath;

    if (StartWith(req->FilePath, "syntax/")) {
        resp.IsPath = true;
        resp.Content = req->FilePath;
        emb_Send_Message_To_CF(FILERESP, &resp);
    }else{
        guiaction.Destination = Parts::GUI;
        guiaction.callback = GotContentFromEditArea;
        guiaction.filename = req->FilePath;
        guiaction.actiontype = GUIAction::GETEDITAREACONTENT;
        core::Interact(&guiaction);
    }
}

static void Draw(Highlight* highlight){
    guiaction.actiontype = GUIAction::DRAW;
    guiaction.filename = highlight->file;
    guiaction.startpos = highlight->startpos;
    guiaction.endpos = highlight->endpos;
    switch (highlight->type) {
    case CF_TYPE:
        guiaction.otherdata = strdup("type");
        break;
    case CF_KEYWORD:
        guiaction.otherdata = strdup("keyword");
        break;
    case CF_MULTCMT:
        guiaction.otherdata = strdup("cmt");
        break;
    case CF_SINGCMT:
        guiaction.otherdata = strdup("scmt");
        break;
    case CF_TEXT:
        guiaction.otherdata = strdup("text");
        break;
    case CF_TAG:
        guiaction.otherdata = strdup("tag");
        break;
    case CF_FUNCTIONNAME:
        guiaction.otherdata = strdup("func");
        break;
    case CF_VALUE:
        guiaction.otherdata = strdup("value");
        break;
    default:
        guiaction.otherdata = strdup("none");
        break;
    }
    core::Interact(&guiaction);
}

static void CfMessageReceiver(MessageType type, void* data){
    /*
     * Receiving messages form CloudyForest
     */
    if(type == CONNECT){
        g_print("CloudyForest Connected\n");
    }else if(type == FILEREQ){
        RequestFile((FileRequest*)data);
    }else if(type == DRAW){
        Highlight *highlight =(Highlight*)data;
        Draw(highlight);
    }
}


void cf::Init(){
    emb_Connect(CfMessageReceiver);
}

void cf::Process(CFAction *action){
    static Lang langmes;
    if (action->actiontype == CFAction::LOADTEMP) {
        langmes.LangName = strdup(action->language.c_str());
        emb_Send_Message_To_CF(MessageType::LANG, &langmes);
    }else if(action->actiontype == CFAction::READFILE){
        emb_Send_Message_To_CF(MessageType::RELOAD, nullptr);
        ent.FileName = strdup(action->filepath.c_str());
        ent.language = strdup(action->language.c_str());
        emb_Send_Message_To_CF(MessageType::ENTRYFILE, &ent);
    }
}
