#ifndef LSPPOPOVERS_H_
#define LSPPOPOVERS_H_

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include <vector>

//forward declaration
typedef struct Suggestion Suggestion;


class TipPopover{
public:
    TipPopover();
    ~TipPopover();
    void setCursorRect(GdkRectangle *cursorrect);
    void ShowContent(const char* content);
    void Hide();
private:
    GdkRectangle *m_cursorRect;
    GtkLabel *m_tipLabel;
    GtkPopover *m_popover;
};


class SuggestionPopover{
public:
    SuggestionPopover();
    ~SuggestionPopover();
    void setTargetBuffer(GtkTextBuffer *targetbuffer);
    void setCursorRect(GdkRectangle *cursorrect);
    void Show();
    void Hide();
    void Add(Suggestion *item);
    void Clear();

private:
    GtkPopover *m_popover;
    GtkScrolledWindow *m_scrollWin;
    GtkBox *m_box;
    GtkWidget *m_selectedItem;

    unsigned int m_selectedItemItr;
    int m_scrolledWinHeight;
    int m_scrolledWinWidth;
    int m_suggItemHeight;
    bool m_isShowing;
    std::vector<std::pair<Suggestion,GtkWidget*>> m_suggestions;
    GtkAdjustment *m_scrolledWindowAdj;

    // from caller
    GtkTextBuffer *m_targetTextBuffer;
    GdkRectangle *m_cursorRect;
    // manual allocated
    GtkTextIter m_startItr, m_endItr;

    void SelectUp();
    void SelectDown();
    void ConfirmSelection();
    void Select(unsigned int itemnum);
    void UnSelectSelected();
};


#endif
