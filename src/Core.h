#ifndef CORE_H_
#define CORE_H_

#include "Global.h"
#include <gtk/gtk.h>
#include <future>
#include <string>

enum class Parts{
    CORE,GUI,PYTHON,CLOUDYFOREST,FILEMANAGER
};

class Action{
public:
    Parts from;
    Parts Destination;
};

class FileAction: public Action{
public:
    enum ActionType{
        OPENFILE, OPENFOLDER, READ, SAVE, ENUMERATE
    };
    ActionType actiontype;
    GFile *file;
    void *data;
    void (*callback) (GFile *file,GFileInfo *fileinfo);
};


class CFAction: public Action{
public:
    enum ActionType{
        LOADTEMP, HIGHLIGHT, READFILE
    };
    ActionType actiontype;
    std::string filepath;
    std::string language;
    void *data;
    void (*callback)();
};

class GUIAction: public Action{
public:
    enum ActionType{
        GETEDITAREACONTENT, DRAWBYLINE, DRAWBYPOS, CHANGEEDITAREAGFILE, ADDTEXTCHANGEDCALLBACK
    };
    ActionType actiontype;
    std::string filename;
    std::string othertext;
    int startpos, endpos;//draw by pos
    int line, offset, length;//draw by line
    void *otherdata;
    void (*callback)(char*);
};

class PyAction: public Action{
public:
    std::string code;//run the code
};

namespace core {
const results::Results* Interact(Action *action);
}

#endif
