#include "DiagnosticTool.h"

#include "datatypes/common.h"
#include "datatypes/lsp.h"
#include "editarea/EditArea.h"
#include "toolset/tools/Tool.h"

DiagnosticTool::DiagnosticTool(EditArea& parent):
    m_parent(parent){
    //
}

DiagnosticTool::~DiagnosticTool(){
    //
    this->Clear();
}

void DiagnosticTool::Add(std::unique_ptr<Diagnostic> diagnostic){
    m_diagnosticsList.emplace_back(std::move(diagnostic));
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
    static const char* tags[5] = {
        "none",// severity must not be 0
        "error",
        "warning",
        "info",
        "hint"
    };

    for (const std::unique_ptr<Diagnostic>& diagnostic : m_diagnosticsList) {
        m_parent.ApplyTagByRange(&diagnostic->range, tags[diagnostic->severity]);
        severityList[diagnostic->severity] ++;
    }

    m_updatedCallback(severityList[1], severityList[2], severityList[3], severityList[4]);
}

void DiagnosticTool::Clear(){
    for (const std::unique_ptr<Diagnostic>& diagnostic : m_diagnosticsList){
        delete [] diagnostic->message;
        delete [] diagnostic->code;
    }

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
