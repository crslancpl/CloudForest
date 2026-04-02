#include "LangPanel.h"

#include <gtk/gtk.h>
#include <vector>

#include "src/gui/Gui_if.h"
#include "EditArea.h"
#include "src/gui/windows/MainWindow.h"
#include "src/filemanagement/FileManagement_if.h"
#include "toolset/tools/text_tool.h"

/* Forward definitions */
static void LangChoosen(GtkButton* self, gpointer data);
static void TemplateEnum(GFile *file, GFileInfo *info);
static void LoadLanguages();
static void ReloadLang(GtkButton* self, void* data);

static GtkWindow *LangChoosingWindow = nullptr;
static EditArea *EditAreaToChongeLang;//
static std::vector<GtkButton*> LangButtons;
static GtkBox* LangButBox;

static void LangChoosen(GtkButton* self, gpointer data){
    EditAreaToChongeLang->setLanguage(gtk_button_get_label(self));
    EditAreaToChongeLang = nullptr;
}

static void TemplateEnum(GFile *file, GFileInfo *info){
    if (file == nullptr ) return;

    std::vector<std::string> d = tools::TrimText(g_file_info_get_name(info), ".");

    GtkButton *newlangbut = GTK_BUTTON(gtk_button_new_with_label(d[0].c_str()));

    LangButtons.push_back(newlangbut);

    gtk_box_append(LangButBox, GTK_WIDGET(newlangbut));

    gtk_widget_add_css_class(GTK_WIDGET(newlangbut), "normalbutton");

    g_signal_connect(newlangbut, "clicked", G_CALLBACK(LangChoosen), nullptr);

    g_object_unref(info);
    g_object_unref(file);
}

static void LoadLanguages(){
    /*
     * Load languages from syntax folder
     */
    GFile *folder = g_file_new_for_path("syntax");
    filemanagement::EnumerateFolderChild(folder, TemplateEnum);
}

static void ReloadLang(GtkButton* self, void* data){
    /*
     * Clear all language buttons from the flyout
     */
    unsigned int offset = 0;
    for(GtkButton* button : LangButtons){
        gtk_box_remove(LangButBox, GTK_WIDGET(button));
    }
    LangButtons.clear();

    LoadLanguages();
}

void langpanel::Construct(){
    /*
     * Create the language choosing window and set it as
     * the flyout of the AppWindow.
     */
    LangChoosingWindow = GTK_WINDOW(gtk_window_new());
    gtk_window_set_hide_on_close(LangChoosingWindow, true);
    gtk_window_set_transient_for(LangChoosingWindow, gui::g_mainwindow->m_window);
    gtk_window_set_title(GTK_WINDOW(LangChoosingWindow), "Choose a language");
    gtk_window_set_default_size(LangChoosingWindow, 200, 100);

    LangButBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));
    gtk_window_set_child(LangChoosingWindow, GTK_WIDGET(LangButBox));

    GtkButton *reloadbutton = GTK_BUTTON(gtk_button_new_with_label("reload"));
    g_signal_connect(reloadbutton, "clicked", G_CALLBACK(ReloadLang), nullptr);
    gtk_box_append(LangButBox, GTK_WIDGET(reloadbutton));

    LoadLanguages();
}

void langpanel::ChooseLanguage(EditArea *editarea){
    EditAreaToChongeLang = editarea;
    gtk_widget_set_visible(GTK_WIDGET(LangChoosingWindow), true);
}
