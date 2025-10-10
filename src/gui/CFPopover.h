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
    GtkLabel *m_Label;
    std::vector<std::pair<Suggestion*,GtkWidget*>> m_Suggestions;
    void Init(GtkWidget *parent);
    void Show(GdkRectangle* rec);
    void Hide();
    void Add(Suggestion *item);
    void Clear();
};

#endif
