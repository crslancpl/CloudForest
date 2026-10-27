#include "AppContext.h"
#include "src/filemanagement/FileManager.h"
#include "src/languages/LanguageManager.h"
#include "src/session/SessionManager.h"
#include "toolset/syntaxprovider/CFCore.h"
#include "pythonbackend/python_if.h"

#include <gtk/gtk.h>
#include <memory>

int main (int argc,char *argv[]){
    AppContext app;

    app.sessionManager = std::make_unique<SessionManager>(app);
    app.langManager = std::make_unique<LanguageManager>(app);
    app.fileManager = std::make_unique<FileManager>(app);
    app.appUI = std::make_unique<AppUI>(app);

    pybackend::Start(app);

    cf::Init(app);// does not depend on other parts of the app

    pybackend::RunEnabledExtensions();

    int status = app.appUI->Run(argc, argv);// this runs a loop for GTK and shows the gui
    pybackend::End();

    return status;
}
