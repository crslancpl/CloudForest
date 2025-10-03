#ifndef EMBED
#define EMBED

extern "C" {

enum cf_MessageType:char{
    CONNECT, DRAW, ERROR, WARN, INFO, DOC, FILEREQ, FILERESP, LANG, ENTRYFILE, RELOAD,
    FIND, TOGGLEOUTPUT
};

typedef struct {
    void (*bridge)(cf_MessageType type,void* data);
}cf_Connect_msg;

typedef struct {
    bool AcceptOutput;
}cf_ToggleOutput_msg;

typedef struct {
    const char* FilePath;//relative
}cf_FileRequest_msg;

typedef struct {
    bool IsPath;
    const char* FilePath;// relative, telling which file is this
    const char* Content;// can be path or whole file
}cf_FileResponse_msg;

typedef struct {
    const char* FilePath;
    const char* Text;
}cf_Find_msg;

typedef struct {
    char* LangName;
}cf_LoadLang_msg;

typedef struct {
    char* FileName;
    char* language;
}cf_Entry_msg;

enum cf_HLType{
    CF_TYPE, CF_KEYWORD, CF_FUNCTIONNAME, CF_NONE, CF_MULTCMT, CF_SINGCMT, CF_TEXT, CF_TAG, CF_VALUE,
    CF_CHAR, CF_NEWLINE, CF_MODIFIER
};

typedef struct {
    char* file;
    unsigned int startpos, endpos;
    unsigned int startline, startlinepos;
    unsigned int endline, endlinepos;
    unsigned int textlength;
    cf_HLType type;
}cf_Highlight_msg;

void cf_Send_Message(cf_MessageType type, void* data);

void cf_Post_Message(cf_MessageType type, void *data);
/*
 * [!NOTE]
 * Post is from cloudyforst to the app
 */

void cf_Request_File(const char* filepath);

void cf_Response_File_Req();

}
#endif
