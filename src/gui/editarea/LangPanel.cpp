#include "LangPanel.h"

#include "datatypes/common.h"
#include "EditArea.h"
#include "Gui_if.h"
#include "components/Flyout.h"
#include "windows/MainWindow.h"
#include "src/languages/LanguageManager_if.h"

#include <glib-object.h>
#include <gtk/gtk.h>

static LangPanel *lang_panel = nullptr;

/*
 * Callbacks
 */
static void OnLangChoosen(GtkButton* self, Language* lang){
    lang_panel->LangChoosen(lang);
}

static void OnFocusLost(GtkEventControllerFocus* self, void* data){
    lang_panel->Hide();
}


LangPanel::LangPanel() : Flyout(gui::GetMainWindow()->GetGtkWindow()){
    /*
     * Create the language choosing window and set it as
     * the flyout of g_mainwindow's GtkWindow.
     */
    this->SetSize(200, 100);
    m_langBtnBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));

    this->SetChild(GTK_WIDGET(m_langBtnBox));

    auto langlist = langmanager::GetLanguageList();

    for(auto itr : langlist){
        this->AddLanguage(itr.second);
    }
}



void LangPanel::AddLanguage(Language* lang){
    GtkButton *newlangbut = GTK_BUTTON(gtk_button_new_with_label(lang->name));
    m_langBtns.push_back(newlangbut);
    gtk_box_append(m_langBtnBox, GTK_WIDGET(newlangbut));
    gtk_widget_add_css_class(GTK_WIDGET(newlangbut), "normal-button");
    g_signal_connect(newlangbut, "clicked", G_CALLBACK(OnLangChoosen), lang);
}

void LangPanel::ChooseFor(EditArea* target){
    m_target = target;
    this->Show();
}

void LangPanel::LangChoosen(Language* lang){
    m_target->SetLanguage(lang);
}

void OpenLangPanelForEditArea(EditArea *target){
    if (!lang_panel) {
        lang_panel = new LangPanel();//freed on app clised
    }
    lang_panel->ChooseFor(target);
}
