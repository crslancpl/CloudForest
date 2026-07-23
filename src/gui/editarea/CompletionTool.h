#ifndef COMPLETION_H_
#define COMPLETION_H_

#include "datatypes/lsp.h"
#include "editarea/CompletionPopover.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <unordered_set>
#include <vector>

// forward declare
class EditArea;



class CompletionTool{
public:
    static bool IsCompletionTriggerChar(char c);
    static std::unordered_set<char> default_skip_char;

    CompletionTool(EditArea& parent);
    ~CompletionTool();

    void Add(std::unique_ptr<Completion> completion);
    void Clear();
    void HidePopover();
    void Request(unsigned int line, unsigned int column);
    void ShowPopover();

    void SelectUp();
    void SelectDown();
    void Confirm();

    bool GetIsShowing();

    std::unique_ptr<CompletionPopover> GetPopoverOwnership();
    void SetPopoverOwnership(std::unique_ptr<CompletionPopover> popover);

private:
    EditArea& m_parent;
    bool m_isShowing;
    std::vector<std::unique_ptr<Completion>> m_completionsList;
    std::unique_ptr<CompletionPopover> m_completionPopover;
};

#endif
