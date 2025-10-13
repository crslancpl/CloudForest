#include "CFPopover.h"
#include <cstdio>
#include <vector>

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

void SuggestionPopover::Init(GtkWidget *parent){
    //m_Label = GTK_LABEL(gtk_label_new("Suggestions: "));
    m_Popover = GTK_POPOVER(gtk_popover_new());
    m_Box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    m_ScrollWin = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());

    gtk_widget_add_css_class(GTK_WIDGET(m_Popover), "suggestionpopover");
    //gtk_label_set_xalign(m_Label, 0);
    gtk_popover_set_child(m_Popover, GTK_WIDGET(m_ScrollWin));
    gtk_scrolled_window_set_child(m_ScrollWin, GTK_WIDGET(m_Box));
    gtk_scrolled_window_set_max_content_height(m_ScrollWin, 70);
    gtk_widget_set_size_request(GTK_WIDGET(m_ScrollWin), 300, 200);
    //gtk_widget_set_can_focus(GTK_WIDGET(m_Popover), false);
    gtk_widget_set_parent(GTK_WIDGET(m_Popover), GTK_WIDGET(parent));
    //gtk_box_append(m_Box, GTK_WIDGET(m_Label));
    gtk_popover_set_position(m_Popover, GTK_POS_BOTTOM);
    gtk_popover_set_autohide(m_Popover, false);
    gtk_popover_set_has_arrow(m_Popover, false);
}

void SuggestionPopover::Show(GdkRectangle* rec){
    gtk_popover_set_pointing_to(m_Popover, rec);
    gtk_popover_popup(m_Popover);
    int width = gtk_widget_get_size(GTK_WIDGET(m_Popover), GTK_ORIENTATION_HORIZONTAL);
    gtk_popover_set_offset(m_Popover, width/2, 5);

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
    m_Suggestions.emplace_back(std::move(item), GTK_WIDGET(button));
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
}

void SuggestionPopover::SelectDown(){
    UnSelectSelected();
    m_SelectedItemItr ++;
    if( m_SelectedItemItr > m_Suggestions.size()-1){
        m_SelectedItemItr = 0;
    }

    Select(m_SelectedItemItr);
}

//private
void SuggestionPopover::Select(unsigned int itemnum){
    m_SelectedItem = m_Suggestions[m_SelectedItemItr].second;
    gtk_widget_add_css_class(m_SelectedItem, "selecteditem");
}

//private
void SuggestionPopover::UnSelectSelected(){
    gtk_widget_remove_css_class(m_SelectedItem, "selecteditem");
}
