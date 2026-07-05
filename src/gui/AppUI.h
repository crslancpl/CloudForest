#ifndef APPUI_H_
#define APPUI_H_

#include <gtk/gtk.h>

class MainWindow;
class HeaderBar;
class SettingPanel;
class FilePanel;
class LangPanel;
class DiagnosticPanel;

typedef struct AppUI{
    GtkApplication *gtkApp;
    MainWindow *mainWindow;
    HeaderBar *headerBar;
    SettingPanel *settingPanel;
    FilePanel *filePanel;
    LangPanel *langPanel;
    DiagnosticPanel *diagnosticPanel;
} AppUI;

#endif
