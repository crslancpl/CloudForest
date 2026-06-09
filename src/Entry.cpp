#include "filemanagement/FileManagement_if.h"
#include "src/languages/LanguageManager_if.h"
#include "toolset/syntaxprovider/syntax_provider.h"
#include "gui/Gui_if.h"
#include "pythonbackend/python_if.h"

#include <gtk/gtk.h>

int main (int argc,char *argv[]){
    filemanagement::Init();// does not depend on other parts of the app
    syntaxprovider::Init();// does not depend on other parts of the app
    langmanager::Init();// connects only static events

    pybackend::Start();
    pybackend::RunEnabledExtensions();

    int status = gui::RunApp(argc, argv);// this runs a loop for GTK and shows the gui

    pybackend::End();

    return status;
}
