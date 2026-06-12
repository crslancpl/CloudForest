#include "Session.h"
#include "datatypes/file.h"

#include <gtk/gtk.h>

void Session::AddWorkspace(Workspace* ws){
    m_workspaces.emplace_back(ws);
}
