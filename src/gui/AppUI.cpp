#include "AppUI.h"

#include "editarea/CompletionPopover.h"
#include "editarea/DiagnosticPanel.h"
#include "layouts/tab/CfTabLayout.h"

#include <gtk/gtk.h>
#include <memory>

AppUI::AppUI(){
    //
}


AppUI::~AppUI(){
    //
}

void AppUI::Start(GtkApplication* gtkapp){
    m_gtkApp = gtkapp;

    m_mainWindow = std::make_unique<MainWindow>(*this);


    std::unique_ptr<HeaderBar> headerbar = std::make_unique<HeaderBar>(*this);
    m_mainWindow->SetHeaderBar(std::move(headerbar));
    m_headerBar = headerbar.get();

    std::unique_ptr<FilePanel> filepanel = std::make_unique<FilePanel>(*this);
    m_filePanel = filepanel.get();
    m_mainWindow->Insert(std::move(filepanel));

    m_settingPanel = std::make_unique<SettingPanel>(*this);

    m_langPanel = std::make_unique<LangPanel>(*this);

    m_fileDialog = GTK_FILE_DIALOG(gtk_file_dialog_new());

    std::unique_ptr<CfTabLayout> tab = std::make_unique<CfTabLayout>();// freed on app closed
    m_mainWindow->Insert(std::move(tab));
}

void AppUI::Finalize(){
    //
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
