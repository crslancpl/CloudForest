#ifndef COMPLETIONPOPOVER_H_
#define COMPLETIONPOPOVER_H_

#include "components/CfComponent.h"
#include "datatypes/lsp.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <vector>

// forward declare
class EditArea;

class CompletionPopoverItem : CfComponent{
public:
    static const int ITEM_HEIGHT = 32;

    CompletionPopoverItem(std::unique_ptr<Completion> completion);
    ~CompletionPopoverItem();

    void SetSelected(bool selected);
    const Completion& GetCompletion();

    GtkWidget* GetBaseWidget() override;

private:
    GtkButton* m_button;
    GtkLabel* m_label;

    std::unique_ptr<Completion> m_completion;
};



class CompletionPopover{
public:
    CompletionPopover();
    ~CompletionPopover();

    void Add(std::unique_ptr<Completion> completion);
    void Clear();
    void Hide();

    const Completion& SelectUp();
    const Completion& SelectDown();
    const Completion& Confirm();

    void SetTarget(EditArea* ea);

    void Show(const GdkRectangle *cursorrec);
    //void Show(const GdkRectangle *cursorrec, const std::vector<std::unique_ptr<Completion>>& completions);

private:
    void ScrollToSelected();

    EditArea* m_target;
    GtkPopover* m_popover;
    GtkScrolledWindow* m_scrolledWindow;
    GtkAdjustment* m_vAdjustment;
    GtkBox* m_box;

    double m_visibleRangeTop;
    double m_visibleRangeBottom;
    int m_selectedItr = -1;
    CompletionPopoverItem* m_selectedItem = nullptr;

    std::vector<std::unique_ptr<CompletionPopoverItem>> m_items;
};


#endif
