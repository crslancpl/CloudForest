#include "DiagnosticTool.h"

#include "AppUI.h"
#include "Gui_if.h"
#include "datatypes/common.h"
#include "datatypes/lsp.h"
#include "editarea/DiagnosticPopover.h"
#include "editarea/EditArea.h"
#include "toolset/tools/Tool.h"

#include <gdk/gdk.h>
#include <memory>

const char* DiagnosticTool::StringifySeverity(int severity){
    static const char* tags[5] = {
        "unknown",// severity must not be 0
        "error",
        "warning",
        "info",
        "hint"
    };
    if (severity >=1 && severity <= 4) {
        return tags[severity];
    } else {
        return tags[0];
    }
}

DiagnosticTool::DiagnosticTool(EditArea& parent):
    m_parent(parent){
    //
}

DiagnosticTool::~DiagnosticTool(){
    //
    this->Clear();
    AppUI &ui = gui::GetCurrentUI();
    if (m_diagnosticPanel) {
        ui.TransferDiagnosticPanel(nullptr);
    }

    if (m_diagnosticPopover) {
        ui.TransferDiagnosticPopover(nullptr);
    }
}

void DiagnosticTool::Add(std::unique_ptr<Diagnostic> diagnostic){
    m_diagnosticsList.emplace_back(std::move(diagnostic));
}

void DiagnosticTool::Clear(){
    m_diagnosticsList.clear();
    m_clearedCallback();
}

Diagnostic* DiagnosticTool::Find(const ZPosition& zpos){
    for (const std::unique_ptr<Diagnostic>& diagnostic : m_diagnosticsList) {
        if(tools::IsZPosInRange(zpos, &diagnostic->range)){
            return diagnostic.get();
        }
    }
    return nullptr;
}

void DiagnosticTool::Process(int version){
    //
    if (version != m_parent.GetFileVersion()) {
        // printf("> file version not the same\n");
        return;
    }

    char severityList[5] = {-1, 0, 0, 0, 0};
    // [0      , 1    , 2      , 3          , 4   ]
    // [Unknown, Error, Warning, Information, Hint]

    for (const std::unique_ptr<Diagnostic>& diagnostic : m_diagnosticsList) {
        m_parent.ApplyTagByRange(&diagnostic->range,
            DiagnosticTool::StringifySeverity(diagnostic->severity));
        severityList[diagnostic->severity] ++;
    }

    m_updatedCallback(severityList[1], severityList[2], severityList[3], severityList[4]);
}

void DiagnosticTool::ShowPanel(){
    if (!m_diagnosticPanel) {
        gui::GetCurrentUI().TransferDiagnosticPanel(this);
    }

    m_diagnosticPanel->ShowFor(&m_parent);
}

void DiagnosticTool::HidePanel(){
    if (m_diagnosticPanel) {
        m_diagnosticPanel->Hide();
    }
}

void DiagnosticTool::ShowPopover(const ZPosition& pos){
    const Diagnostic* diag = this->Find(pos);
    if (diag) {
        if (!m_diagnosticPopover) {
            gui::GetCurrentUI().TransferDiagnosticPopover(this);
        }
        if ( diag == m_showingDiagnositc) {
            return;
        }
        m_showingDiagnositc = diag;
        GdkRectangle rec = m_parent.CalculatePositionRectangle(diag->range.start);
        m_diagnosticPopover->Show(*diag, rec);
    } else {
        this->HidePopover();
    }
}

void DiagnosticTool::HidePopover(){
    if (m_diagnosticPopover) {
        m_diagnosticPopover->Hide();
    }
    m_showingDiagnositc = nullptr;
}

std::unique_ptr<DiagnosticPopover> DiagnosticTool::GetPopoverOwnership(){
    return std::move(m_diagnosticPopover);
}

void DiagnosticTool::SetPopoverOwnership(std::unique_ptr<DiagnosticPopover> popover){
    m_diagnosticPopover = std::move(popover);
    m_diagnosticPopover->SetTarget(&m_parent);
}

std::unique_ptr<DiagnosticPanel> DiagnosticTool::GetPanelOwnership(){
    return std::move(m_diagnosticPanel);
}

void DiagnosticTool::SetPanelOwnership(std::unique_ptr<DiagnosticPanel> panel){
    m_diagnosticPanel = std::move(panel);
}


const std::vector<std::unique_ptr<Diagnostic>> &DiagnosticTool::GetDiagnostics(){
    return m_diagnosticsList;
}


// callbacks

void DiagnosticTool::OnCleared(std::function<void()> callback){
    m_clearedCallback = callback;
}

void DiagnosticTool::OnUpdated(std::function<void(int,int,int,int)> callback){
    m_updatedCallback = callback;
}
