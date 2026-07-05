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

static AppUI* app_ui = nullptr;

typedef struct {
    LangPanel* parent;
    Language* lang;
} LangChoosenData;

/*
 * Callbacks
 */
static void OnLangChoosen(GtkButton* self, LangChoosenData* data){
    data->parent->LangChoosen(data->lang);
}

static void OnFocusLost(GtkEventControllerFocus* self, void* data){
    if (app_ui) {
        app_ui->langPanel->Hide();
    }
}

LangPanel::LangPanel(AppUI& appui) : Flyout(appui.mainWindow->GetGtkWindow())
    , m_appUI(appui)
    {
    /*
     * Create the language choosing window and set it as
     * the flyout of g_mainwindow's GtkWindow.
     */
    appui.langPanel = this;
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
    LangChoosenData *data = new LangChoosenData();
    data->lang = lang;
    data->parent = this;
    g_signal_connect(newlangbut, "clicked", G_CALLBACK(OnLangChoosen), data);
}

void LangPanel::ChooseFor(EditArea* target){
    m_target = target;
    this->Show();
}

void LangPanel::LangChoosen(Language* lang){
    m_target->SetLanguage(lang);
}

void OpenLangPanelForEditArea(EditArea *target){
    if (!app_ui->langPanel) {
        return;
    }
    app_ui->langPanel->ChooseFor(target);
}
