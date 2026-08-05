#include "LangPanel.h"

#include "datatypes/common.h"
#include "EditArea.h"
#include "AppUI.h"
#include "Gui_if.h"
#include "components/Flyout.h"
#include "headerbar/Headerbar.h"
#include "windows/MainWindow.h"
#include "src/languages/LanguageManager_if.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <memory>
#include <string>
#include <vector>

/*
 * Callbacks
 */
static void on_lang_item_clicked(GtkButton* self, LangPanelItem* parent){
    parent->Select();
}

static void on_focus_lost(GtkEventControllerFocus* self, LangPanel* langpanel){
    langpanel->Hide();
}

/*
 * LangPanelItem class
 */

LangPanelItem::LangPanelItem(const Language& lang):m_lang(lang){
    m_button = GTK_BUTTON(gtk_button_new());
    gtk_widget_set_size_request(GTK_WIDGET(m_button), 160, 60);

    GtkBox *box1 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_button_set_child(m_button, GTK_WIDGET(box1));

    GtkBox *box2 = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_box_append(box1, GTK_WIDGET(box2));

    m_nameLabel = GTK_LABEL(gtk_label_new(lang.name));
    gtk_widget_add_css_class(GTK_WIDGET(m_nameLabel), "name-label");
    gtk_box_append(box2, GTK_WIDGET(m_nameLabel));

    m_idLabel = GTK_LABEL(gtk_label_new(lang.id));
    gtk_widget_add_css_class(GTK_WIDGET(m_idLabel), "id-label");
    gtk_box_append(box2, GTK_WIDGET(m_idLabel));

    std::string fileextensions;
    for (const std::string& ext : m_lang.fileExtensions) {
        fileextensions.append(".");
        fileextensions.append(ext);
        fileextensions.append(" ");
    }

    m_fileExtLabel = GTK_LABEL(gtk_label_new(fileextensions.c_str()));
    gtk_label_set_xalign(m_fileExtLabel, 0);
    gtk_widget_set_overflow(GTK_WIDGET(m_fileExtLabel), GTK_OVERFLOW_HIDDEN);
    gtk_widget_set_size_request(GTK_WIDGET(m_fileExtLabel), 100, 30);
    gtk_label_set_wrap(m_fileExtLabel, true);
    //gtk_label_set_wrap_mode(m_fileExtLabel, )
    gtk_widget_add_css_class(GTK_WIDGET(m_fileExtLabel), "ext-label");
    gtk_box_append(box1, GTK_WIDGET(m_fileExtLabel));

    g_signal_connect(m_button, "clicked", G_CALLBACK(on_lang_item_clicked), this);
}

LangPanelItem::~LangPanelItem(){
    //
}

void LangPanelItem::Select(){
    this->SetState(true);
    m_selectedCallback(*this);
}

void LangPanelItem::SetState(bool selected){
    if (selected) {
        gtk_widget_add_css_class(GTK_WIDGET(m_button), "selected");
    } else {
        gtk_widget_remove_css_class(GTK_WIDGET(m_button), "selected");
    }
}

const Language& LangPanelItem::GetLanguage() const{
    return m_lang;
}

void LangPanelItem::OnSelected(std::function<void(LangPanelItem&)> callback){
    m_selectedCallback = callback;
}

GtkWidget* LangPanelItem::GetBaseWidget(){
    return GTK_WIDGET(m_button);
}

/*
 * LangPanel class
 */

LangPanel::LangPanel(AppUI& appui) : Flyout(appui.GetMainWindow()->GetGtkWindow())
    , m_appUI(appui)
    {
    /*
     * Create the language choosing window and set it as
     * the flyout of g_mainwindow's GtkWindow.
     */
    this->SetSize(486, 300);
    gtk_widget_add_css_class(GTK_WIDGET(m_flyoutWindow), "lang-panel");
    m_grid = GTK_GRID(gtk_grid_new());
    m_scrolledWindow = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new());

    gtk_scrolled_window_set_child(m_scrolledWindow, GTK_WIDGET(m_grid));

    this->SetChild(GTK_WIDGET(m_scrolledWindow));

    const std::unordered_map<std::string, Language*>& langmap = langmanager::GetLanguageMap();

    for(auto namelangpair : langmap){
        this->AddLanguage(namelangpair.second);
    }
}



void LangPanel::AddLanguage(const Language* lang){
    std::unique_ptr<LangPanelItem> item = std::make_unique<LangPanelItem>(*lang);

    item->OnSelected([&](LangPanelItem& langitem){
        if (m_selectedItem) {
            m_selectedItem->SetState(false);
        }
        m_target->SetLanguage(&langitem.GetLanguage());
        m_selectedItem = &langitem;
    });

    gtk_grid_attach(m_grid, GTK_WIDGET(item->GetBaseWidget()), m_nextItemCol, m_nextItemRow, 1, 1);
    m_items.push_back(std::move(item));
    m_nextItemRow ++;
    if (m_nextItemRow > 2) {
        m_nextItemRow = 0;
        m_nextItemCol ++;
    }
}

void LangPanel::ChooseFor(EditArea* target){
    m_target = target;
    if (m_selectedItem) {
        m_selectedItem->SetState(false);
    }

    for (const std::unique_ptr<LangPanelItem>& item : m_items) {
        if (&item->GetLanguage() == target->GetLanguage()) {
            item->SetState(true);
            m_selectedItem = item.get();
        }
    }
    this->Show();
}
