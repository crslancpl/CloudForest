#include "LangPanel.h"

#include "EditArea.h"
#include "src/gui/Gui_if.h"
#include "src/gui/windows/MainWindow.h"
#include "src/languages/LanguageManager_if.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <vector>

static GtkWindow *lang_choosing_window = nullptr;
static EditArea *editarea_to_chonge_lang;//
static std::vector<GtkButton*> lang_buttons;
static GtkBox* lang_but_box;
static GtkEventController *focus_event_ctrl;

static void LangChoosen(GtkButton* self, void* data){
    editarea_to_chonge_lang->SetLanguage((Language*)data);
}

static void FocusLost(GtkEventControllerFocus* self, void* data){
    gtk_widget_set_visible(GTK_WIDGET(lang_choosing_window), false);
}

static void CreateButton(Language* lang){
    GtkButton *newlangbut = GTK_BUTTON(gtk_button_new_with_label(lang->name.c_str()));
    lang_buttons.push_back(newlangbut);
    gtk_box_append(lang_but_box, GTK_WIDGET(newlangbut));
    gtk_widget_add_css_class(GTK_WIDGET(newlangbut), "normalbutton");
    g_signal_connect(newlangbut, "clicked", G_CALLBACK(LangChoosen), lang);
}

void langpanel::Construct(){
    /*
     * Create the language choosing window and set it as
     * the flyout of g_mainwindow->m_window.
     */
    lang_choosing_window = GTK_WINDOW(gtk_window_new());
    focus_event_ctrl = gtk_event_controller_focus_new();
    gtk_window_set_transient_for(lang_choosing_window, gui::g_mainwindow->GetGtkWindow());
    gtk_window_set_decorated(lang_choosing_window, false);
    gtk_window_set_default_size(lang_choosing_window, 200, 100);
    g_signal_connect(focus_event_ctrl, "leave", G_CALLBACK(FocusLost), nullptr);
    gtk_widget_add_controller(GTK_WIDGET(lang_choosing_window), focus_event_ctrl);

    lang_but_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));
    gtk_window_set_child(lang_choosing_window, GTK_WIDGET(lang_but_box));

    langmanager::ListenEvent(langmanager::LANG_MANAGER_NEW_LANG, (void(*)())CreateButton);
}

void langpanel::ChooseLanguage(EditArea *editarea){
    editarea_to_chonge_lang = editarea;
    gtk_widget_set_visible(GTK_WIDGET(lang_choosing_window), true);
}
