#ifndef COMPLETION_H_
#define COMPLETION_H_

#include "datatypes/lsp.h"

#include <gtk/gtk.h>
#include <memory>
#include <vector>

// forward declare
class EditArea;



class CompletionPopoverItem{
public:
    CompletionPopoverItem(const Completion& completion);
    ~CompletionPopoverItem();

private:
    GtkButton* m_button;
};

class CompletionPopover{
public:
    CompletionPopover();
    ~CompletionPopover();

private:
    GtkPopover* m_popover;

    std::vector<std::unique_ptr<CompletionPopoverItem>> m_items;
};

class CompletionTool{
public:
    CompletionTool(EditArea& parent);
    ~CompletionTool();

    void Request(unsigned int line, unsigned int column);
    void New(std::unique_ptr<Completion> completion);

private:
    EditArea& m_parent;
    std::vector<std::unique_ptr<Completion>> m_completionsList;
    std::unique_ptr<CompletionPopover> m_completionPopover;
};

#endif
