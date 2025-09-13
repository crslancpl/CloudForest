#include "CFCore.h"

#include <cstring>
#include <gtk/gtk.h>
#include <map>
#include <future>

#include "../Core.h"
#include "../ToolFunctions.h"
#include "CFEmbed.h"

static FileRespond fresp;
static Entry ent;


static void RequestFile(FileRequest *freq){
    fresp.FilePath = freq->FilePath;

    if (StartWith(freq->FilePath, "syntax/")) {
        fresp.IsPath = true;
        fresp.Content = freq->FilePath;
        emb_Send_Message_To_CF(FILERESP, &fresp);
    }else{
        static request::EAGetText req;
        req.Filepath = freq->FilePath;

        std::future<const result::Result*> gettext = std::async(core::Interact, &req);
        result::GetText* text = (result::GetText*)gettext.get();

        fresp.Content = text->Text->c_str();
        fresp.IsPath = false;
        emb_Send_Message_To_CF(FILERESP, &fresp);
    }
}

static void Draw(Highlight* highlight){
    static request::EADrawByPos req;
    static map<t, std::string> tagnames = {
        {CF_TYPE,"type"},{CF_KEYWORD, "keyword"},{CF_SINGCMT, "scmt"},{CF_MULTCMT, "cmt"},
        {CF_TEXT, "text"},{CF_TAG, "tag"},{CF_FUNCTIONNAME, "func"},{CF_VALUE, "value"},
        {CF_NONE, "none"}
    };

    req.Startpos = highlight->startpos;
    req.Endpos = highlight->endpos;
    req.Filepath = highlight->file;
    auto tagname = tagnames.find(highlight->type)->second;
    req.Tagname = tagname;
    core::Interact(&req);
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

void cf::Process(request::Request* request){
    static Lang langmes;
    if(auto req = dynamic_cast<request::CFLoadTemplate*>(request)){
        langmes.LangName = strdup(req->Language.c_str());
        emb_Send_Message_To_CF(MessageType::LANG, &langmes);
    }else if(auto req = dynamic_cast<request::CFReadFile*>(request)){
        emb_Send_Message_To_CF(MessageType::RELOAD, nullptr);
        ent.FileName = strdup(req->Filepath.c_str());
        ent.language = strdup(req->Language.c_str());
        emb_Send_Message_To_CF(MessageType::ENTRYFILE, &ent);
    }
}
