#ifndef EMBED
#define EMBED

extern "C" {

enum MessageType:char{
    CONNECT, DRAW, ERROR, WARN, INFO, DOC, FILEREQ, FILERESP, LANG, ENTRYFILE, RELOAD
};

typedef struct Connect{
    void (*bridge)(MessageType type,void* data);
}Connect ;

typedef struct FileRequest{
    /*
     * from cloudyforest to the app
     */
    const char* FilePath;//relative
}FileRequest;

typedef struct FileRespond{
    bool IsPath;
    const char* FilePath;// relative, telling which file is this
    const char* Content;// can be path or whole file
}FileRespond;

typedef struct Lang{
    char* LangName;
}Lang;

typedef struct Entry{
    char* FileName;
    char* language;
}Entry;

enum t{
    CF_TYPE, CF_KEYWORD, CF_FUNCTIONNAME, CF_NONE, CF_MULTCMT, CF_SINGCMT, CF_TEXT, CF_TAG, CF_VALUE
};

typedef struct Highlight{
    char* file;
    int startpos;
    int endpos;
    t type;
}Highlight;

void emb_Send_Message_To_CF(MessageType type, void* data);

void emb_Connect(void (*func)(MessageType, void*));

}
#endif
