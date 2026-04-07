#include "CFCore.h"

#include <glib.h>
#include <string>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <unordered_map>


#include "toolset/tools/text_tool.h"
#include "src/gui/editarea/EditArea_if.h"
#include "src/gui/editarea/EditArea.h"
#include "CFEmbed.h"


static cf_FileResponse_msg fresp;
static cf_Entry_msg ent;


static void RequestFile(cf_FileRequest_msg *freq){
    fresp.FilePath = freq->FilePath;
    if (tools::StartWith(freq->FilePath, "syntax/")) {
        fresp.IsPath = true;
        fresp.Content = strcat(strdup(std::string("data/").c_str()) , freq->FilePath);
        cf_Send_Message(FILERESP, &fresp);
    }else{
        auto targetEditArea = editarea::FindEditArea(freq->FilePath);
        if(!targetEditArea){
            return;
        }
        auto text = targetEditArea->getContent();

        fresp.Content = text;
        fresp.IsPath = false;
        cf_Send_Message(FILERESP, &fresp);
    }
}

static void Draw(cf_Highlight_msg* highlight){
    //static request::EADrawByPos req;
    static std::pmr::unordered_map<cf_HLType, const char*> tagnames = {
        {CF_TYPE, "type"},{CF_KEYWORD, "keyword"},{CF_SINGCMT, "scmt"},{CF_MULTCMT, "mcmt"},
        {CF_TEXT, "text"},{CF_TAG, "tag"},{CF_FUNCTIONNAME, "func"},{CF_VALUE, "value"},
        {CF_NEWLINE, "none"}, {CF_CHAR, "char"},{CF_NONE, "none"}, {CF_MODIFIER, "tag"}
    };

    auto targetEditArea = editarea::FindEditArea(highlight->file);
    auto tagname = tagnames.find(highlight->type)->second;

    targetEditArea->ApplyTagByLinePos(
        highlight->startline, highlight->startlinepos, highlight->textlength, tagname
    );
}

static void CfMessageReceiver(cf_MessageType type, void* data){
    /*
     * Receiving messages form CloudyForest
     */
    if(type == CONNECT){
        g_print("CloudyForest Connected\n");
    }else if(type == FILEREQ){
        RequestFile((cf_FileRequest_msg*)data);
    }else if(type == DRAW){;
        cf_Highlight_msg *highlight =(cf_Highlight_msg*)data;
        Draw(highlight);
    }else if(type == LANGUAGESERVER){
        /*
        static PyOpenLanguageServer req;
        cf_LanguageServer_msg *ls = (cf_LanguageServer_msg*)data;
        req.LanguageServerCommand = ls->LanguageServerCommand;
        req.CommandOption = ls->CommandOption;

        core::Interact(&req);
        */
    }
}


void cf::Init(){
    cf_Connect_msg m;
    m.bridge = CfMessageReceiver;
    cf_Send_Message(cf_MessageType::CONNECT, &m);
}

void cf::LoadTemplate(const char *langname){
    static cf_LoadLang_msg langmes;
    langmes.LangName = strdup(langname);
    cf_Send_Message(cf_MessageType::LANG, &langmes);
}

void cf::ReadFile(const char *filepath, const char *lang){
    cf::LoadTemplate(lang);
    cf_Send_Message(cf_MessageType::RELOAD, nullptr);
    ent.FileName = strdup(filepath);
    ent.Language = strdup(lang);
    cf_Send_Message(cf_MessageType::ENTRYFILE, &ent);
}
