#include "CFPopover.h"
#include "EditArea.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <string.h>


void TipPopover::Init(GtkWidget *parent){
    m_TipLabel = GTK_LABEL(gtk_label_new(""));
    m_Popover = GTK_POPOVER(gtk_popover_new());
    gtk_widget_add_css_class(GTK_WIDGET(m_Popover), "tippopover");
    gtk_popover_set_child(m_Popover, GTK_WIDGET(m_TipLabel));
    gtk_widget_set_can_focus(GTK_WIDGET(m_Popover), false);
    gtk_widget_set_parent(GTK_WIDGET(m_Popover), GTK_WIDGET(parent));
    gtk_popover_set_position(m_Popover, GTK_POS_RIGHT);
    gtk_popover_set_autohide(m_Popover, false);
    gtk_popover_set_offset(m_Popover, 10, 0);
}

void TipPopover::ShowContent(GdkRectangle* rec, const std::string &content){
    gtk_label_set_label(m_TipLabel, content.c_str());
    gtk_popover_set_pointing_to(m_Popover, rec);
    gtk_popover_popup(m_Popover);
}

void TipPopover::Hide(){
    gtk_popover_popdown(m_Popover);
}


/*
 * SuggestionPopover
 */

void SuggestionPopover::Init(EditArea *parent){
    m_Parent = parent;
    m_Popover = GTK_POPOVER(gtk_popover_new());
    m_Box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    m_ScrollWin = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    m_ScrolledWindowAdj = gtk_scrolled_window_get_vadjustment(m_ScrollWin);
    m_SuggItemHeight = 30;

    // [!NOTE]
    // The item height cannot be lower than 30 or the scrolling
    // will be bugged.

    gtk_widget_add_css_class(GTK_WIDGET(m_Popover), "suggestionpopover");


    gtk_scrolled_window_set_child(m_ScrollWin, GTK_WIDGET(m_Box));
    gtk_scrolled_window_set_max_content_height(m_ScrollWin, m_SuggItemHeight);
    gtk_widget_set_size_request(GTK_WIDGET(m_ScrollWin), 400, 180);

    gtk_widget_set_parent(GTK_WIDGET(m_Popover), GTK_WIDGET(parent->m_TextView));

    gtk_popover_set_child(m_Popover, GTK_WIDGET(m_ScrollWin));
    gtk_popover_set_position(m_Popover, GTK_POS_BOTTOM);
    gtk_popover_set_autohide(m_Popover, false);
    gtk_popover_set_has_arrow(m_Popover, false);
}

void SuggestionPopover::Show(GdkRectangle* rec){
    gtk_popover_set_pointing_to(m_Popover, rec);
    gtk_widget_get_size_request(GTK_WIDGET(m_ScrollWin), &m_ScrolledWinWidth, &m_ScrolledWinHeight);
    gtk_popover_set_offset(m_Popover, m_ScrolledWinWidth/2 ,10);
    gtk_popover_popup(m_Popover);

    m_SelectedItemItr = 0;
    Select(m_SelectedItemItr);// select the first one by default

    m_IsShowing = true;
}

void SuggestionPopover::Hide(){
    gtk_popover_popdown(m_Popover);
    m_IsShowing = false;
}

void SuggestionPopover::Add(Suggestion *item){
    if (item == nullptr) {
        g_print("null suggestion\n");
    }

    GtkButton *button = GTK_BUTTON(gtk_button_new());
    GtkLabel *label = GTK_LABEL(gtk_label_new(item->Label.c_str()));
    gtk_label_set_xalign(label, 0);
    gtk_button_set_child(button, GTK_WIDGET(label));
    gtk_widget_add_css_class(GTK_WIDGET(button),"normalbutton");
    gtk_widget_set_size_request(GTK_WIDGET(button), 0, m_SuggItemHeight);
    m_Suggestions.emplace_back(std::move(*item), GTK_WIDGET(button));
    gtk_box_append(m_Box, GTK_WIDGET(button));
}

void SuggestionPopover::Clear(){
    for(auto item : m_Suggestions ){
        gtk_box_remove(m_Box, item.second);
    }
    m_Suggestions.clear();
    Hide();
}

void SuggestionPopover::SelectUp(){
    UnSelectSelected();
    if( m_SelectedItemItr == 0){
        m_SelectedItemItr = m_Suggestions.size()-1;
    }else{
        m_SelectedItemItr --;
    }

    Select(m_SelectedItemItr);
    int width = gtk_widget_get_size(GTK_WIDGET(m_Popover), GTK_ORIENTATION_HORIZONTAL);
    gtk_popover_set_offset(m_Popover, width/2, 5);
}

void SuggestionPopover::SelectDown(){
    UnSelectSelected();
    m_SelectedItemItr ++;
    if( m_SelectedItemItr > m_Suggestions.size()-1){
        m_SelectedItemItr = 0;
    }

    Select(m_SelectedItemItr);
    int width = gtk_widget_get_size(GTK_WIDGET(m_Popover), GTK_ORIENTATION_HORIZONTAL);
    gtk_popover_set_offset(m_Popover, width/2, 5);
}

void SuggestionPopover::ConfirmSelection(){
    m_Parent->m_PauseTextChangedCallback = true;
    Suggestion sug = m_Suggestions[m_SelectedItemItr].first;
    std::string sugtext = sug.InsertText + " ";
    gtk_text_buffer_get_iter_at_line_offset(
        m_Parent->m_TextViewBuffer, m_Parent->StartItr,sug.Range.StartLine, sug.Range.StartChar);

    gtk_text_buffer_get_iter_at_line_offset(
        m_Parent->m_TextViewBuffer, m_Parent->EndItr,sug.Range.EndLine, sug.Range.EndChar);

    gtk_text_buffer_delete(m_Parent->m_TextViewBuffer, m_Parent->StartItr, m_Parent->EndItr);

    m_Parent->m_PauseTextChangedCallback = false;

    gtk_text_buffer_insert(m_Parent->m_TextViewBuffer,
        m_Parent->StartItr, sugtext.c_str(), sugtext.size());
}

//private
void SuggestionPopover::Select(unsigned int itemnum){
    /*
     * The Height of every Completion button is fixed to
     * 22 if the app is not scaled. The button's height
     * is retrieved with gtk_widget_get_height().
     */
    m_SelectedItem = m_Suggestions[m_SelectedItemItr].second;

    gtk_widget_remove_css_class(m_SelectedItem, "normalitem");
    gtk_widget_add_css_class(m_SelectedItem, "selecteditem");

    double adjvalue = gtk_adjustment_get_value(m_ScrolledWindowAdj);
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
    int rangebottom = adjvalue + m_ScrolledWinHeight;

    /*
     * m_SelectedItemItr * m_SuggItemHeight
     *  = location of the top of the selected item button
     *
     * m_SelectedItemItr * m_SuggItemHeight + m_SuggItemHeight
     *  = location of the bottom of the selected item button
     */

    int itemtoppos = m_SelectedItemItr * m_SuggItemHeight;
    int itembottompos = m_SelectedItemItr * m_SuggItemHeight + m_SuggItemHeight;




    if(itembottompos > rangebottom){
        // scroll down
        gtk_adjustment_set_value(m_ScrolledWindowAdj, itembottompos - m_ScrolledWinHeight);
    }else if(itemtoppos < rangetop){
        // scroll up
        gtk_adjustment_set_value(m_ScrolledWindowAdj, itemtoppos);
    }
}

//private
void SuggestionPopover::UnSelectSelected(){
    gtk_widget_remove_css_class(m_SelectedItem, "selecteditem");
    gtk_widget_add_css_class(m_SelectedItem, "normalitem");
}
