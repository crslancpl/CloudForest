#ifndef CFPOPOVER_H_
#define CFPOPOVER_H_

#include "gtk/gtk.h"

#include <string>
#include <vector>

class TipPopover{
public:
    GtkLabel *TipLabel;
    GtkPopover *Popover;
    void Init(GtkWidget *parent);
    void ShowContent(GdkRectangle* rec, const std::string &content);
    void Hide();
};

class Suggestion{
public:
    std::string Text;
    std::string Code;
    std::string Type;
    std::string Description;
};

class SuggestionPopover{
public:
    GtkPopover *Popover;
    GtkScrolledWindow *ScrollWin;
    GtkBox *Box;
    GtkLabel *Label;
    void Init(GtkWidget *parent);
    void Show(GdkRectangle* rec, const std::vector<Suggestion> *content);
    void Hide();
};

#endif
