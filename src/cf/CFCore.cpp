#include "CFCore.h"

#include <cstring>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <map>
#include <future>

#include "../Core.h"
#include "../ToolFunctions.h"
#include "CFEmbed.h"

#include "../requests/BaseRequest.h"
#include "../requests/GUIRequests.h"
#include "../requests/CFRequests.h"

static cf_FileResponse_msg fresp;
static cf_Entry_msg ent;


static void RequestFile(cf_FileRequest_msg *freq){
    fresp.FilePath = freq->FilePath;

    if (StartWith(freq->FilePath, "syntax/")) {
        fresp.IsPath = true;
        fresp.Content = freq->FilePath;
        cf_Send_Message(FILERESP, &fresp);
    }else{
        static EAGetText req;
        req.Filepath = freq->FilePath;

        std::future<const result::Result*> gettext = std::async(core::Interact, &req);
        result::GetText* text = (result::GetText*)gettext.get();

        fresp.Content = text->Text;
        fresp.IsPath = false;
        cf_Send_Message(FILERESP, &fresp);
    }
}

static void Draw(cf_Highlight_msg* highlight){
    //static request::EADrawByPos req;
    static EADrawByLine req;
    static map<cf_HLType, std::string> tagnames = {
        {CF_TYPE, "type"},{CF_KEYWORD, "keyword"},{CF_SINGCMT, "scmt"},{CF_MULTCMT, "mcmt"},
        {CF_TEXT, "text"},{CF_TAG, "tag"},{CF_FUNCTIONNAME, "func"},{CF_VALUE, "value"},
        {CF_NEWLINE, "none"}, {CF_CHAR, "char"},{CF_NONE, "none"}, {CF_MODIFIER, "tag"}
    };

    req.Line = highlight->startline;
    req.Offset = highlight->startlinepos;
    req.Length = highlight->textlength;
    req.Filepath = highlight->file;
    auto tagname = tagnames.find(highlight->type)->second;
    req.Tagname = tagname;
    core::Interact(&req);
}

static void CfMessageReceiver(cf_MessageType type, void* data){
    /*
     * Receiving messages form CloudyForest
     */
    if(type == CONNECT){
        g_print("CloudyForest Connected\n");
    }else if(type == FILEREQ){
        RequestFile((cf_FileRequest_msg*)data);
    }else if(type == DRAW){
        cf_Highlight_msg *highlight =(cf_Highlight_msg*)data;
        Draw(highlight);
    }
}


void cf::Init(){
    cf_Connect_msg m;
    m.bridge = CfMessageReceiver;
    cf_Send_Message(cf_MessageType::CONNECT, &m);
}

void cf::Process(Request* request){
    static cf_LoadLang_msg langmes;
    if(auto req = dynamic_cast<CFLoadTemplate*>(request)){
        langmes.LangName = strdup(req->Language.c_str());
        cf_Send_Message(cf_MessageType::LANG, &langmes);
    }else if(auto req = dynamic_cast<CFReadFile*>(request)){
        cf_Send_Message(cf_MessageType::RELOAD, nullptr);
        ent.FileName = strdup(req->Filepath.c_str());
        ent.language = strdup(req->Language.c_str());
        cf_Send_Message(cf_MessageType::ENTRYFILE, &ent);
    }
}
