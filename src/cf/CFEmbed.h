#ifndef EMBED
#define EMBED

extern "C" {

enum MessageType:char{
    CONNECT, DRAW, ERROR, WARN, INFO, DOC, FILEREQ, FILERESP, LANG, ENTRYFILE, RELOAD
};

/*
 * Remember to free the message after receive it in you app
 */

typedef struct Message{
    MessageType Type;
    void *Data;
}Message;

typedef struct Connect{
    void (*bridge)(Message* m);
}Connect ;

typedef struct FileRequest{
    const char* FilePath;//reletive
}FileRequest;

typedef struct FileRespond{
    bool IsPath;
    const char* FilePath;// reletive, telling which file is this
    const char* Content;// can be path or whole file
}FileRespond;

typedef struct Lang{
    char* LangName;
}Lang;

typedef struct Entry{
    char* FileName;
}Entry;

enum t{
    CF_TYPE, CF_KEYWORD, CF_FUNCTIONNAME, CF_NONE
};

typedef struct Highlight{
    char* file;
    int startpos;
    int endpos;
    t type;
}Highlight;

void emb_Send_Message_To_CF(Message* mes);

void emb_Connect(void (*func)(Message*));

void emb_Send_Message(Message *m);

void emb_Request_File(const char* filepath);

void emb_Respond_File_Req();

}
#endif
