#ifndef CORE_H_
#define CORE_H_

#include "gui/EditArea.h"
#include <gtk/gtk.h>
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
        GETEDITAREACONTENT, DRAW, CHANGEEDITAREAGFILE
    };
    ActionType actiontype;
    std::string filename;
    int startpos, endpos;
    void *otherdata;
    void (*callback)(char*);
};

namespace core {
void *Interact(Action *action);
}

#endif
