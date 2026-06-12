#ifndef SESSION_H_
#define SESSION_H_

#include "datatypes/file.h"

#include <gtk/gtk.h>
#include <list>

class Session {
public:
    void AddWorkspace(Workspace* ws);

private:
    const char* m_workingPath;
    std::list<Workspace*> m_workspaces;
};


#endif
