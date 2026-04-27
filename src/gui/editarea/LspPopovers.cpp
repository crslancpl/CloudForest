#include "LspPopovers.h"

#include "datatypes/lsp.h"

TipPopover::TipPopover(){
    m_tipLabel = GTK_LABEL(gtk_label_new(""));
    m_popover = GTK_POPOVER(gtk_popover_new());
    gtk_widget_add_css_class(GTK_WIDGET(m_popover), "tippopover");
    gtk_popover_set_child(m_popover, GTK_WIDGET(m_tipLabel));
    gtk_widget_set_can_focus(GTK_WIDGET(m_popover), false);
    gtk_popover_set_position(m_popover, GTK_POS_RIGHT);
    gtk_popover_set_autohide(m_popover, false);
    gtk_popover_set_offset(m_popover, 10, 0);
}

TipPopover::~TipPopover(){
    g_object_unref(m_popover);
}

void TipPopover::setCursorRect(GdkRectangle *cursorrect){
    m_cursorRect = cursorrect;
}

void TipPopover::ShowContent(const char* content){
    gtk_label_set_label(m_tipLabel, content);
    gtk_popover_set_pointing_to(m_popover, m_cursorRect);
    gtk_popover_popup(m_popover);
}

void TipPopover::Hide(){
    gtk_popover_popdown(m_popover);
}


/*
 * SuggestionPopover
 */

SuggestionPopover::SuggestionPopover(){
    m_popover = GTK_POPOVER(gtk_popover_new());
    m_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    m_scrollWin = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    m_scrolledWindowAdj = gtk_scrolled_window_get_vadjustment(m_scrollWin);
    m_suggItemHeight = 30;


    // [!NOTE]
    // The item height cannot be lower than 30 or the scrolling
    // will be bugged.

    gtk_widget_add_css_class(GTK_WIDGET(m_popover), "suggestionpopover");


    gtk_scrolled_window_set_child(m_scrollWin, GTK_WIDGET(m_box));
    gtk_scrolled_window_set_max_content_height(m_scrollWin, m_suggItemHeight);
    gtk_widget_set_size_request(GTK_WIDGET(m_scrollWin), 400, 180);

    gtk_popover_set_child(m_popover, GTK_WIDGET(m_scrollWin));
    gtk_popover_set_position(m_popover, GTK_POS_BOTTOM);
    gtk_popover_set_autohide(m_popover, false);
    gtk_popover_set_has_arrow(m_popover, false);
}

SuggestionPopover::~SuggestionPopover(){
    g_object_unref(m_popover);
}

void SuggestionPopover::setCursorRect(GdkRectangle *cursorrect){
    m_cursorRect = cursorrect;
}

void SuggestionPopover::setTargetBuffer(GtkTextBuffer *targetbuffer){
    /*
     * This has to be called when focus switched
     */
    m_targetTextBuffer = targetbuffer;
}

void SuggestionPopover::Show(){
    gtk_popover_set_pointing_to(m_popover, m_cursorRect);
    gtk_widget_get_size_request(GTK_WIDGET(m_scrollWin), &m_scrolledWinWidth, &m_scrolledWinHeight);
    gtk_popover_set_offset(m_popover, m_scrolledWinWidth/2 ,10);
    gtk_popover_popup(m_popover);

    m_selectedItemItr = 0;
    Select(m_selectedItemItr);// select the first one by default

    m_isShowing = true;
}

void SuggestionPopover::Hide(){
    gtk_popover_popdown(m_popover);
    m_isShowing = false;
}

void SuggestionPopover::Add(Suggestion *item){
    if (item == nullptr) {
        g_print("null suggestion\n");
    }

    GtkButton *button = GTK_BUTTON(gtk_button_new());
    GtkLabel *label = GTK_LABEL(gtk_label_new(item->label.c_str()));
    gtk_label_set_xalign(label, 0);
    gtk_button_set_child(button, GTK_WIDGET(label));
    gtk_widget_add_css_class(GTK_WIDGET(button),"normalbutton");
    gtk_widget_set_size_request(GTK_WIDGET(button), 0, m_suggItemHeight);
    m_suggestions.emplace_back(std::move(*item), GTK_WIDGET(button));
    gtk_box_append(m_box, GTK_WIDGET(button));
}

void SuggestionPopover::Clear(){
    for(auto item : m_suggestions ){
        gtk_box_remove(m_box, item.second);
    }
    m_suggestions.clear();
    Hide();
}

void SuggestionPopover::SelectUp(){
    UnSelectSelected();
    if( m_selectedItemItr == 0){
        m_selectedItemItr = m_suggestions.size()-1;
    }else{
        m_selectedItemItr --;
    }

    Select(m_selectedItemItr);
    int width = gtk_widget_get_size(GTK_WIDGET(m_popover), GTK_ORIENTATION_HORIZONTAL);
    gtk_popover_set_offset(m_popover, width/2, 5);
}

void SuggestionPopover::SelectDown(){
    UnSelectSelected();
    m_selectedItemItr ++;
    if( m_selectedItemItr > m_suggestions.size()-1){
        m_selectedItemItr = 0;
    }

    Select(m_selectedItemItr);
    int width = gtk_widget_get_size(GTK_WIDGET(m_popover), GTK_ORIENTATION_HORIZONTAL);
    gtk_popover_set_offset(m_popover, width/2, 5);
}

void SuggestionPopover::ConfirmSelection(){
    // trigger suggestion selected event
    Suggestion sug = m_suggestions[m_selectedItemItr].first;
    std::string sugtext = sug.insertText + " ";
    gtk_text_buffer_get_iter_at_line_offset(
        m_targetTextBuffer, &m_startItr,sug.range.startLine, sug.range.startColumn);

    gtk_text_buffer_get_iter_at_line_offset(
        m_targetTextBuffer, &m_endItr,sug.range.endLine, sug.range.endColumn);

    gtk_text_buffer_delete(m_targetTextBuffer, &m_startItr, &m_endItr);

    //m_Parent->m_pauseTextChangedCallback = false;

    gtk_text_buffer_insert(m_targetTextBuffer,
        &m_startItr, sugtext.c_str(), sugtext.size());
}

//private
void SuggestionPopover::Select(unsigned int itemnum){
    /*
     * The Height of every Completion button is fixed to
     * 22 if the app is not scaled. The button's height
     * is retrieved with gtk_widget_get_height().
     */
    m_selectedItem = m_suggestions[m_selectedItemItr].second;

    gtk_widget_remove_css_class(m_selectedItem, "normalitem");
    gtk_widget_add_css_class(m_selectedItem, "selecteditem");

    double adjvalue = gtk_adjustment_get_value(m_scrolledWindowAdj);
    /*
     * rangetop is the position of the scrolled window's adjustment.
     * It is indicating the top of the scrolled window. So the we have
     * to calculate the rangebottom with `rangetop + scrolledwindow's height`
     *
     * ------------ top
     * item
     * item
     * ------------ bottom
     */


    int rangetop = adjvalue;
    int rangebottom = adjvalue + m_scrolledWinHeight;

    /*
     * m_SelectedItemItr * m_SuggItemHeight
     *  = location of the top of the selected item button
     *
     * m_SelectedItemItr * m_SuggItemHeight + m_SuggItemHeight
     *  = location of the bottom of the selected item button
     */

    int itemtoppos = m_selectedItemItr * m_suggItemHeight;
    int itembottompos = m_selectedItemItr * m_suggItemHeight + m_suggItemHeight;




    if(itembottompos > rangebottom){
        // scroll down
        gtk_adjustment_set_value(m_scrolledWindowAdj, itembottompos - m_scrolledWinHeight);
    }else if(itemtoppos < rangetop){
        // scroll up
        gtk_adjustment_set_value(m_scrolledWindowAdj, itemtoppos);
    }
}

//private
void SuggestionPopover::UnSelectSelected(){
    gtk_widget_remove_css_class(m_selectedItem, "selecteditem");
    gtk_widget_add_css_class(m_selectedItem, "normalitem");
}
