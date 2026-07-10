#ifndef APPUI_H_
#define APPUI_H_

#include "headerbar/Headerbar.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "filepanel/FilePanel.h"
#include "editarea/LangPanel.h"
#include "editarea/DiagnosticPanel.h"


#include <gtk/gtk.h>
#include <memory>

typedef struct AppUI{
    GtkApplication *gtkApp;
    GtkFileDialog *fileDialog;// for filemanager;
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<HeaderBar> headerBar;
    std::unique_ptr<SettingPanel> settingPanel;
    std::unique_ptr<FilePanel> filePanel;
    std::unique_ptr<LangPanel> langPanel;
    std::unique_ptr<DiagnosticPanel> diagnosticPanel;
} AppUI;

#endif
