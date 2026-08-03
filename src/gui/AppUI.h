#ifndef APPUI_H_
#define APPUI_H_

#include "editarea/CompletionPopover.h"
#include "editarea/CompletionTool.h"
#include "editarea/DiagnosticTool.h"
#include "headerbar/Headerbar.h"
#include "windows/MainWindow.h"
#include "settingpanel/SettingPanel.h"
#include "filepanel/FilePanel.h"
#include "editarea/DiagnosticPanel.h"
#include "editarea/LangPanel.h"


#include <gtk/gtk.h>
#include <memory>

typedef struct AppUI_{
    GtkApplication *gtkApp;
    GtkFileDialog *fileDialog;// for filemanager;
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<DiagnosticPanel> diagnosticPanel;
    std::unique_ptr<SettingPanel> settingPanel;
    std::unique_ptr<LangPanel> langPanel;

    //borrow reference
    HeaderBar* headerBar;
    FilePanel* filePanel;
} AppUI_;


class AppUI{
public:
    AppUI();
    ~AppUI();

    void Start(GtkApplication* gtkapp);
    void Finalize();

    MainWindow* GetMainWindow();
    FilePanel* GetFilePanel();
    SettingPanel* GetSettingPanel();
    DiagnosticPanel* GetDiagnosticPanel();
    LangPanel* GetLangPanel();
    GtkFileDialog* GetFileDialog();

    GtkApplication* GetGtkApp();

    void TransferDiagnosticPanel(DiagnosticTool* newowner);
    void TransferDiagnosticPopover(DiagnosticTool* newowner);
    void TransferCompletionPopover(CompletionTool* newowner);

private:
    std::unique_ptr<MainWindow> m_mainWindow;

    std::unique_ptr<CompletionPopover> m_completionPopover;
    CompletionTool* m_completionPopoverOwner = nullptr;
    std::unique_ptr<DiagnosticPanel> m_diagnosticPanel;
    DiagnosticTool* m_diagnosticPanelOwner = nullptr;
    std::unique_ptr<DiagnosticPopover> m_diagnosticPopover;
    DiagnosticTool* m_diagnosticPopoverOwner = nullptr;
    std::unique_ptr<SettingPanel> m_settingPanel;
    std::unique_ptr<LangPanel> m_langPanel;
    FilePanel* m_filePanel;
    HeaderBar* m_headerBar;

    GtkFileDialog *m_fileDialog;// for filemanager;
    GtkApplication* m_gtkApp;
};

#endif
