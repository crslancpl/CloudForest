#ifndef DIAGNOSTICTOOL_H_
#define DIAGNOSTICTOOL_H_

#include "datatypes/common.h"
#include "datatypes/lsp.h"
#include "editarea/DiagnosticPanel.h"
#include "editarea/DiagnosticPopover.h"

#include <functional>
#include <memory>
#include <vector>

// Forward declare
class EditArea;

class DiagnosticTool{
public:
    DiagnosticTool(EditArea& parent);
    ~DiagnosticTool();

    void Add(std::unique_ptr<Diagnostic> diagnostic);
    void Clear();
    Diagnostic* Find(const ZPosition& zpos);
    void Process(int version);

    void ShowPanel();
    void HidePanel();
    void ShowPopover(const ZPosition& zpos);
    void HidePopover();

    std::unique_ptr<DiagnosticPopover> GetPopoverOwnership();
    void SetPopoverOwnership(std::unique_ptr<DiagnosticPopover> popover);
    std::unique_ptr<DiagnosticPanel> GetPanelOwnership();
    void SetPanelOwnership(std::unique_ptr<DiagnosticPanel> panel);

    const std::vector<std::unique_ptr<Diagnostic>> &GetDiagnostics();

    // events
    void OnCleared(std::function<void()> callback);
    void OnUpdated(std::function<void(int,int,int,int)> callback);

private:
    EditArea& m_parent;
    const Diagnostic* m_showingDiagnositc;
    std::function<void()> m_clearedCallback;
    std::function<void(int,int,int,int)> m_updatedCallback;
    std::vector<std::unique_ptr<Diagnostic>> m_diagnosticsList;
    std::unique_ptr<DiagnosticPopover> m_diagnosticPopover;
    std::unique_ptr<DiagnosticPanel> m_diagnosticPanel;
};

#endif
