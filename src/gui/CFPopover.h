#ifndef CFPOPOVER_H_
#define CFPOPOVER_H_

#include <gtk/gtk.h>

#include <string>
#include <vector>

#include "../Types.h"

class TipPopover{
public:
    GtkLabel *TipLabel;
    GtkPopover *Popover;
    void Init(GtkWidget *parent);
    void ShowContent(GdkRectangle* rec, const std::string &content);
    void Hide();
};


class SuggestionPopover{
public:
    GtkPopover *Popover;
    GtkScrolledWindow *ScrollWin;
    GtkBox *Box;
    GtkLabel *Label;
    std::vector<std::pair<Suggestion,GtkWidget*>> Suggestions;
    void Init(GtkWidget *parent);
    void Show(GdkRectangle* rec);
    void Hide();
    void Add(Suggestion &item);
    void Clear();
};

#endif
