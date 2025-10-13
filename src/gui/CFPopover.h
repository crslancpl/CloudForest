#ifndef CFPOPOVER_H_
#define CFPOPOVER_H_

#include <gtk/gtk.h>

#include <string>
#include <vector>

#include "../Types.h"

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
    //GtkLabel *m_Label;
    unsigned int m_SelectedItemItr;
    bool m_IsShowing;
    std::vector<std::pair<Suggestion*,GtkWidget*>> m_Suggestions;

    void Init(GtkWidget *parent);

    void Show(GdkRectangle* rec);
    void Hide();
    void Add(Suggestion *item);
    void Clear();

    void SelectUp();
    void SelectDown();
private:
    void Select(unsigned int itemnum);
    void UnSelectSelected();
};

#endif
