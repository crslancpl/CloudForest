#include "AppUI.h"

#include "editarea/CompletionPopover.h"
#include "editarea/DiagnosticPanel.h"
#include "filepanel/FilePanelButtons.h"
#include "layouts/tab/CfTabLayout.h"
#include "style/Style.h"
#include "src/session/EditAreaData.h"

#include <cstdio>
#include <gtk/gtk.h>
#include <memory>


static void app_activated (GtkApplication *gtkapp, AppUI* app){
    app->OnActivated();
}

static void app_closed (GtkApplication *gtkapp, AppUI* app){
    app->Finalize();
    printf("\nGtk application closed\n");
}

/*
 * AppUI
 */

AppUI::AppUI(AppContext& context) :
    m_context(context)
    {
    //
}


AppUI::~AppUI(){
    //
}

int AppUI::Run(int argc, char* argv[]){
    m_gtkApp = gtk_application_new ("ide.cf", G_APPLICATION_NON_UNIQUE);

    g_signal_connect(m_gtkApp, "activate", G_CALLBACK(app_activated), this);
    g_signal_connect(m_gtkApp, "shutdown", G_CALLBACK(app_closed), this);
    int status = g_application_run (G_APPLICATION (m_gtkApp), argc, argv);// loop
    g_object_unref (m_gtkApp);

    return status;
}

void AppUI::Finalize(){
    //
    this->TransferCompletionPopover(nullptr);
    m_completionPopover.reset();

    this->TransferDiagnosticPopover(nullptr);
    m_diagnosticPopover.reset();

    this->TransferDiagnosticPanel(nullptr);
    m_diagnosticPanel.reset();

    m_settingPanel.reset();
    m_langPanel.reset();
    m_mainWindow.reset();
}

MainWindow* AppUI::GetMainWindow(){
    return m_mainWindow.get();
}

FilePanel* AppUI::GetFilePanel(){
    return m_filePanel;
}

SettingPanel* AppUI::GetSettingPanel(){
    return m_settingPanel.get();
}

DiagnosticPanel* AppUI::GetDiagnosticPanel(){
    return m_diagnosticPanel.get();
}

LangPanel* AppUI::GetLangPanel(){
    return m_langPanel.get();
}

GtkFileDialog* AppUI::GetFileDialog(){
    return m_fileDialog;
}

GtkApplication* AppUI::GetGtkApp(){
    return m_gtkApp;
}

void AppUI::TransferDiagnosticPanel(DiagnosticTool* newowner){
    if (m_diagnosticPanelOwner) {
        m_diagnosticPanel = m_diagnosticPanelOwner->GetPanelOwnership();
    } else if (!m_diagnosticPanel) {
        m_diagnosticPanel = std::make_unique<DiagnosticPanel>(*this);
    }

    if (newowner) {
        newowner->SetPanelOwnership(std::move(m_diagnosticPanel));
        m_diagnosticPanelOwner = newowner;
    }
}

void AppUI::TransferDiagnosticPopover(DiagnosticTool* newowner){
    if (m_diagnosticPopoverOwner) {
        m_diagnosticPopover = m_diagnosticPopoverOwner->GetPopoverOwnership();
    } else if (!m_diagnosticPopover) {
        m_diagnosticPopover = std::make_unique<DiagnosticPopover>();
    }

    if (newowner) {
        newowner->SetPopoverOwnership(std::move(m_diagnosticPopover));
        m_diagnosticPopoverOwner = newowner;
    }
}

void AppUI::TransferCompletionPopover(CompletionTool* newowner){
    if (m_completionPopoverOwner) {
        m_completionPopover = m_completionPopoverOwner->GetPopoverOwnership();
    } else if (!m_completionPopover) {
        m_completionPopover = std::make_unique<CompletionPopover>();
    }

    if (newowner) {
        newowner->SetPopoverOwnership(std::move(m_completionPopover));
        m_completionPopoverOwner = newowner;
    }
}

void AppUI::OnActivated(){
    style::LoadCssFolder("data/styles/default-dark");
    session::InitEditAreaData(m_context);

    m_mainWindow = std::make_unique<MainWindow>(*this);

    std::unique_ptr<HeaderBar> headerbar = std::make_unique<HeaderBar>(m_context);
    m_mainWindow->SetHeaderBar(std::move(headerbar));
    m_headerBar = headerbar.get();

    std::unique_ptr<FilePanel> filepanel = std::make_unique<FilePanel>(m_context);
    m_filePanel = filepanel.get();
    m_mainWindow->Insert(std::move(filepanel));

    m_settingPanel = std::make_unique<SettingPanel>(*this);

    m_langPanel = std::make_unique<LangPanel>(m_context);

    m_fileDialog = GTK_FILE_DIALOG(gtk_file_dialog_new());

    std::unique_ptr<CfTabLayout> tab = std::make_unique<CfTabLayout>();// freed on app closed
    m_mainWindow->Insert(std::move(tab));

    session::EditNewFile();

    m_mainWindow->Show();
}
