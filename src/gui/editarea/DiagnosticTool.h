#ifndef DIAGNOSTICTOOL_H_
#define DIAGNOSTICTOOL_H_

#include "datatypes/common.h"
#include "datatypes/lsp.h"

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

    const std::vector<std::unique_ptr<Diagnostic>> &GetDiagnostics();

    // events
    void OnCleared(std::function<void()> callback);
    void OnUpdated(std::function<void(int,int,int,int)> callback);

private:
    EditArea& m_parent;
    std::function<void()> m_clearedCallback;
    std::function<void(int,int,int,int)> m_updatedCallback;
    std::vector<std::unique_ptr<Diagnostic>> m_diagnosticsList;
};

#endif
