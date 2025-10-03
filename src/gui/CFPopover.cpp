#include "CFPopover.h"
#include <cstdio>
#include <vector>

void TipPopover::Init(GtkWidget *parent){
    TipLabel = GTK_LABEL(gtk_label_new(""));
    Popover = GTK_POPOVER(gtk_popover_new());
    gtk_widget_add_css_class(GTK_WIDGET(Popover), "tippopover");
    gtk_popover_set_child(Popover, GTK_WIDGET(TipLabel));
    gtk_widget_set_can_focus(GTK_WIDGET(Popover), false);
    gtk_widget_set_parent(GTK_WIDGET(Popover), GTK_WIDGET(parent));
    gtk_popover_set_position(Popover, GTK_POS_RIGHT);
    gtk_popover_set_autohide(Popover, false);
    gtk_popover_set_offset(Popover, 10, 0);
}

void TipPopover::ShowContent(GdkRectangle* rec, const std::string &content){
    gtk_label_set_label(TipLabel, content.c_str());
    gtk_popover_set_pointing_to(Popover, rec);
    gtk_popover_popup(Popover);
}

void TipPopover::Hide(){
    gtk_popover_popdown(Popover);
}

void SuggestionPopover::Init(GtkWidget *parent){
    Label = GTK_LABEL(gtk_label_new("Suggestions: "));
    Popover = GTK_POPOVER(gtk_popover_new());
    Box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    ScrollWin = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());

    gtk_widget_add_css_class(GTK_WIDGET(Popover), "suggestionpopover");
    gtk_label_set_xalign(Label, 0);
    gtk_popover_set_child(Popover, GTK_WIDGET(ScrollWin));
    gtk_scrolled_window_set_child(ScrollWin, GTK_WIDGET(Box));
    gtk_scrolled_window_set_max_content_height(ScrollWin, 70);
    gtk_widget_set_size_request(GTK_WIDGET(ScrollWin), 200, 100);
    gtk_widget_set_can_focus(GTK_WIDGET(Popover), false);
    gtk_widget_set_parent(GTK_WIDGET(Popover), GTK_WIDGET(parent));
    gtk_box_append(Box, GTK_WIDGET(Label));
    for(int i = 1; i < 8; i++){
        char text[50];
        sprintf(text, "option %i", i);
        GtkLabel *l = GTK_LABEL(gtk_label_new(text));

        gtk_label_set_xalign(l, 0);
        gtk_box_append(Box, GTK_WIDGET(l));
    }
    gtk_popover_set_position(Popover, GTK_POS_BOTTOM);
    gtk_popover_set_autohide(Popover, false);
    gtk_popover_set_has_arrow(Popover, false);
}

void SuggestionPopover::Show(GdkRectangle* rec){
    gtk_popover_set_pointing_to(Popover, rec);
    gtk_popover_popup(Popover);
    int width = gtk_widget_get_size(GTK_WIDGET(Popover), GTK_ORIENTATION_HORIZONTAL);
    gtk_popover_set_offset(Popover, width/2, 5);
}

void SuggestionPopover::Hide(){
    gtk_popover_popdown(Popover);
}

void SuggestionPopover::Add(Suggestion &item){
    GtkButton *button = GTK_BUTTON(gtk_button_new());
    GtkLabel *label = GTK_LABEL(gtk_label_new(item.Label.c_str()));
    gtk_button_set_child(button, GTK_WIDGET(label));
    Suggestions.emplace_back(std::move(item), GTK_WIDGET(button));
    gtk_box_append(Box, GTK_WIDGET(button));
}

void SuggestionPopover::Clear(){
    for(auto item : Suggestions ){
        gtk_box_remove(Box, item.second);
    }
    Suggestions.clear();
}
