#ifndef CORE_H_
#define CORE_H_

#include <gtk/gtk.h>

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
        OPENFILE, OPENFOLDER, READ, CREATE, ENUMERATE
    };
    ActionType actiontype;
    void *data;
    void (*callback) (GFile *file,GFileInfo *fileinfo);
};

namespace core {
void *Interact(Action *action);
}

#endif
