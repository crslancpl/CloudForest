#include "CompletionPopover.h"

#include "EditArea.h"
#include "Gui_if.h"
#include "datatypes/lsp.h"

#include <gdk/gdk.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>

CompletionPopoverItem::CompletionPopoverItem(std::unique_ptr<Completion> completion)
    :m_completion(std::move(completion)){
    m_label = GTK_LABEL(gtk_label_new(m_completion->label.c_str()));
    gtk_label_set_xalign(m_label, 0);
    m_button = GTK_BUTTON(gtk_button_new());
    gtk_button_set_child(m_button, GTK_WIDGET(m_label));
    gtk_widget_set_size_request(GTK_WIDGET(m_button), 0, 30);
    gtk_widget_set_hexpand(GTK_WIDGET(m_button), true);
}

CompletionPopoverItem::~CompletionPopoverItem(){
    //
}

void CompletionPopoverItem::SetSelected(bool selected){
    if (selected) {
        gtk_widget_add_css_class(GTK_WIDGET(m_button), "selected");
    } else {
        gtk_widget_remove_css_class(GTK_WIDGET(m_button), "selected");
    }
}

const Completion& CompletionPopoverItem::GetCompletion(){
    return *m_completion.get();
}

GtkWidget* CompletionPopoverItem::GetBaseWidget(){
    return GTK_WIDGET(m_button);
}


/*
 * CompletionPopover
 */

CompletionPopover::CompletionPopover(){
    m_popover = GTK_POPOVER(gtk_popover_new());
    g_object_ref(m_popover);
    gtk_widget_set_size_request(GTK_WIDGET(m_popover), 400, 310);
    m_scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());
    m_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_popover_set_child(m_popover, GTK_WIDGET(m_scrolledWindow));
    gtk_scrolled_window_set_child(m_scrolledWindow, GTK_WIDGET(m_box));
    m_vAdjustment = gtk_scrolled_window_get_vadjustment(m_scrolledWindow);
    gtk_popover_set_autohide(m_popover, false);
    gtk_popover_set_has_arrow(m_popover, false);
    gtk_popover_set_position(m_popover, GTK_POS_RIGHT);
    gtk_popover_set_offset(m_popover, 0, 170);
    gtk_widget_add_css_class(GTK_WIDGET(m_popover), "completion-popover");
}

CompletionPopover::~CompletionPopover(){
    //
    g_object_unref(m_popover);
}


void CompletionPopover::Add(std::unique_ptr<Completion> completion){
    std::unique_ptr<CompletionPopoverItem> item = std::make_unique<CompletionPopoverItem>(std::move(completion));
    gtk_box_append(m_box, item->GetBaseWidget());
    m_items.emplace_back(std::move(item));
}

void CompletionPopover::Clear(){
    for (const std::unique_ptr<CompletionPopoverItem>& item : m_items) {
        gtk_box_remove(m_box, item->GetBaseWidget());
    }

    m_items.clear();
}

void CompletionPopover::Hide(){
    gtk_popover_popdown(m_popover);
}

const Completion& CompletionPopover::SelectUp(){
    if (m_selectedItem) {
        m_selectedItem->SetSelected(false);
    }

    m_selectedItr -= 1;
    if (m_selectedItr < 0) {
        m_selectedItr = m_items.size() - 1;
    }

    m_selectedItem = m_items[m_selectedItr].get();
    m_selectedItem->SetSelected(true);
    this->ScrollToSelected();
    return m_selectedItem->GetCompletion();
}

const Completion& CompletionPopover::SelectDown(){
    if (m_selectedItem) {
        m_selectedItem->SetSelected(false);
    }

    m_selectedItr += 1;
    if (m_selectedItr >= m_items.size()) {
        m_selectedItr = 0;
    }

    m_selectedItem = m_items[m_selectedItr].get();
    m_selectedItem->SetSelected(true);
    this->ScrollToSelected();
    return m_selectedItem->GetCompletion();
}

const Completion& CompletionPopover::Confirm(){
    return m_selectedItem->GetCompletion();
}

void CompletionPopover::SetTarget(EditArea* ea){
    this->Clear();
    if (ea != m_target) {
        m_target = ea;
        gtk_widget_set_parent(GTK_WIDGET(m_popover), GTK_WIDGET(ea->GetTextView()));
    }
}

void CompletionPopover::Show(const GdkRectangle* cursorrec){
    gtk_popover_set_pointing_to(m_popover, cursorrec);
    gtk_popover_popup(m_popover);
}
/*
void CompletionPopover::Show(const GdkRectangle* cursorrec, const std::vector<std::unique_ptr<Completion>>& completions){
    this->Clear();

    if (m_target && completions.size() != 0) {

        for (const std::unique_ptr<Completion>& comp : completions) {
            //
            std::unique_ptr<CompletionPopoverItem> item = std::make_unique<CompletionPopoverItem>(*comp.get());
            gtk_box_append(m_box, item->GetBaseWidget());
            m_items.emplace_back(std::move(item));
        }

        gtk_popover_set_pointing_to(m_popover, cursorrec);
        gtk_popover_popup(m_popover);
    } else {
        gtk_popover_popdown(m_popover);
    }
}
*/


// private

inline void CompletionPopover::ScrollToSelected(){
    // item button height = 32
    double upper = gtk_adjustment_get_value(m_vAdjustment);
    double lower = upper + 8 * CompletionPopoverItem::ITEM_HEIGHT;
    int selected = m_selectedItr * CompletionPopoverItem::ITEM_HEIGHT;
    // printf("upper %f lower %f selected %i\n", upper, lower, selected);
    if (selected > lower) {
        //
        gtk_adjustment_set_value(m_vAdjustment, (m_selectedItr - 8) * CompletionPopoverItem::ITEM_HEIGHT);
    } else if (selected < upper) {
        //
        gtk_adjustment_set_value(m_vAdjustment, m_selectedItr * CompletionPopoverItem::ITEM_HEIGHT);
    }
}
