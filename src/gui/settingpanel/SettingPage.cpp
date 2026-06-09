#include "SettingPage.h"

#include <gtk/gtk.h>
#include <pango/pango-layout.h>

static inline GtkLabel* CreateLabel(const char* text){
    // Create a label that align the text to left
    GtkLabel* label = GTK_LABEL(gtk_label_new(text));
    gtk_label_set_xalign(label, 0);
    return label;
}

SettingPage::SettingPage(){
    m_baseBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
    gtk_widget_set_hexpand(GTK_WIDGET(m_baseBox), true);
    gtk_widget_add_css_class(GTK_WIDGET(m_baseBox), "setting-page");
    this->SetContentWidget(GTK_WIDGET(m_baseBox));
    this->SetHorizontalExpand(true);
    this->SetVerticalExpand(true);
}


SettingPage::~SettingPage(){
    //
}

GtkLabel* SettingPage::AddHeader1(const char* text){
    GtkLabel* header1 = CreateLabel(text);
    gtk_widget_add_css_class(GTK_WIDGET(header1), "header1");
    gtk_box_append(m_baseBox, GTK_WIDGET(header1));
    return header1;
}

GtkLabel* SettingPage::AddHeader2(const char* text){
    GtkLabel* header2 = CreateLabel(text);
    gtk_widget_add_css_class(GTK_WIDGET(header2), "header2");
    gtk_box_append(m_baseBox, GTK_WIDGET(header2));
    return header2;
}

GtkLabel* SettingPage::AddParagraph(const char* text){
    GtkLabel* paragraph = CreateLabel(text);
    gtk_widget_add_css_class(GTK_WIDGET(paragraph), "paragraph");
    gtk_label_set_wrap(paragraph, true);
    gtk_label_set_wrap_mode(paragraph, PANGO_WRAP_WORD);
    gtk_label_set_xalign(paragraph, 0);
    gtk_box_append(m_baseBox, GTK_WIDGET(paragraph));
    return paragraph;
}

void SettingPage::AddWidget(GtkWidget* child){
    gtk_box_append(m_baseBox, child);
}
