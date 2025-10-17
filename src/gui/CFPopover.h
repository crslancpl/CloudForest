#ifndef CFPOPOVER_H_
#define CFPOPOVER_H_

#include <gtk/gtk.h>

#include <string>
#include <vector>

#include "../Types.h"
#include "EditArea.h"

class TipPopover{
public:
    GtkLabel *m_TipLabel;
    GtkPopover *m_Popover;
    void Init(GtkWidget *parent);
    void ShowContent(GdkRectangle* rec, const std::string &content);
    void Hide();
};


class SuggestionPopover{
public:
    GtkPopover *m_Popover;
    GtkScrolledWindow *m_ScrollWin;
    GtkBox *m_Box;
    GtkWidget *m_SelectedItem;
    EditArea *m_Parent;
    unsigned int m_SelectedItemItr;
    int m_ScrolledWinHeight;
    int m_ScrolledWinWidth;
    int m_SuggItemHeight;
    bool m_IsShowing;
    std::vector<std::pair<Suggestion,GtkWidget*>> m_Suggestions;

    void Init(EditArea *parent);

    void Show(GdkRectangle* rec);
    void Hide();
    void Add(Suggestion *item);
    void Clear();

    void SelectUp();
    void SelectDown();
    void ConfirmSelection();
private:
    GtkAdjustment *m_ScrolledWindowAdj;
    void Select(unsigned int itemnum);
    void UnSelectSelected();
};

#endif
