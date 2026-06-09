#include "ToggleBlock.h"
#include <glib-object.h>
#include <gtk/gtk.h>
#include <pango/pango-layout.h>

static void OnSwitchStateSet(GtkSwitch* self, bool state, ToggleBlock* parent){
    parent->SetState(state);
}

ToggleBlock::ToggleBlock(const char* name, const char* description, bool state){
    m_baseGrid = GTK_GRID(gtk_grid_new());
    m_nameLabel = GTK_LABEL(gtk_label_new(name));
    m_descriptionLabel = GTK_LABEL(gtk_label_new(description));
    m_switchBtn = GTK_SWITCH(gtk_switch_new());

    gtk_grid_attach(m_baseGrid, GTK_WIDGET(m_nameLabel), 0, 0, 1, 1);
    gtk_grid_attach(m_baseGrid, GTK_WIDGET(m_switchBtn), 1, 0, 1, 1);
    gtk_grid_attach(m_baseGrid, GTK_WIDGET(m_descriptionLabel), 0, 1, 2, 1);

    gtk_widget_add_css_class(GTK_WIDGET(m_baseGrid), "toggle-block");
    gtk_widget_add_css_class(GTK_WIDGET(m_nameLabel), "toggle-block-name");
    gtk_widget_add_css_class(GTK_WIDGET(m_descriptionLabel), "paragraph");
    gtk_widget_set_hexpand(GTK_WIDGET(m_baseGrid), true);
    gtk_widget_set_hexpand(GTK_WIDGET(m_nameLabel), true);
    gtk_widget_set_hexpand(GTK_WIDGET(m_descriptionLabel), true);
    gtk_label_set_wrap(m_descriptionLabel, true);
    gtk_label_set_wrap_mode(m_descriptionLabel, PANGO_WRAP_WORD);
    gtk_label_set_max_width_chars(m_descriptionLabel, 30);
    gtk_label_set_xalign(m_nameLabel, 0);
    gtk_label_set_xalign(m_descriptionLabel, 0);

    g_signal_connect(m_switchBtn, "state-set", G_CALLBACK(OnSwitchStateSet), this);

    this->SetState(state);
}

ToggleBlock::~ToggleBlock(){
    //
}

void ToggleBlock::SetName(const char* name){
    gtk_label_set_text(m_nameLabel, name);
}

void ToggleBlock::SetDescription(const char* description){
    gtk_label_set_text(m_descriptionLabel, description);
}

void ToggleBlock::SetState(bool state){
    gtk_switch_set_active(m_switchBtn, state);
    gtk_switch_set_state(m_switchBtn, state);
    m_toggleState = state;
}

void ToggleBlock::Toggle(){
    this->SetState(!m_toggleState);
}


GtkWidget* ToggleBlock::GetBaseWidget(){
    return GTK_WIDGET(m_baseGrid);
}
