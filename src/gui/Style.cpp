#include "Style.h"
#include "guiCore.h"
#include "../ToolFunctions.h"
#include <pango/pango-attributes.h>
#include <vector>



void style::LoadCssFromPath(const std::string &path){
    GtkCssProvider *CssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(CssProvider, path.c_str());
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(CssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}


void style::LoadTextTag(GtkTextBuffer *buffer){
    gtk_text_buffer_create_tag(buffer, "type", "foreground","rgb(165,229,112)", nullptr);
    gtk_text_buffer_create_tag(buffer, "keyword", "foreground","cyan", nullptr);
    gtk_text_buffer_create_tag(buffer, "none", "foreground","white", nullptr);
    gtk_text_buffer_create_tag(buffer, "char", "foreground","white", nullptr);
    gtk_text_buffer_create_tag(buffer, "mcmt", "foreground","rgb(58,193,114)", nullptr);
    gtk_text_buffer_create_tag(buffer, "scmt", "foreground","orange", nullptr);
    gtk_text_buffer_create_tag(buffer, "text", "foreground","magenta", nullptr);
    gtk_text_buffer_create_tag(buffer, "tag", "foreground","rgb(176,112,229)", nullptr);
    gtk_text_buffer_create_tag(buffer, "func", "foreground","rgb(199,176,252)", nullptr);
    gtk_text_buffer_create_tag(buffer, "value", "foreground","rgb(128,188,237)", nullptr);

    gtk_text_buffer_create_tag(buffer, "error", "underline", PANGO_UNDERLINE_ERROR, nullptr);
    gtk_text_buffer_create_tag(buffer, "warning", "underline", PANGO_UNDERLINE_SINGLE, nullptr);

    gtk_text_buffer_create_tag(buffer, "search_highlight",
                                "background", "yellow",
                                "foreground", "black",
                                nullptr);
}



/*
 * Language choosing panel
 */

static GtkWindow *LangChoosingWindow = nullptr;
static EditArea *EditAreaToChongeLang;//
static std::vector<GtkButton*> LangButtons;
static GtkBox* LangButBox;


static void LangChoosen(GtkButton* self, gpointer data){
    EditAreaToChongeLang ->ChangeLanguage(gtk_button_get_label(self),true);
}

static void TemplateEnum(GFile *file, GFileInfo *info){
    if (file == nullptr ) return;

    vector<string> d = TrimText(g_file_info_get_name(info), ".");

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
    gui::EnumFolder(folder, TemplateEnum);
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



void style::InitLangChooser(){
    /*
     * Create the language choosing window and set it as
     * the flyout of the AppWindow.
     */
    LangChoosingWindow = GTK_WINDOW(gtk_window_new());
    gtk_window_set_hide_on_close(LangChoosingWindow, true);
    gtk_window_set_transient_for(LangChoosingWindow, gui::AppWindow.Window);
    gtk_window_set_title(GTK_WINDOW(LangChoosingWindow), "Choose a language");
    gtk_window_set_default_size(LangChoosingWindow, 200, 100);

    LangButBox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));
    gtk_window_set_child(LangChoosingWindow, GTK_WIDGET(LangButBox));

    GtkButton *reloadbutton = GTK_BUTTON(gtk_button_new_with_label("reload"));
    g_signal_connect(reloadbutton, "clicked", G_CALLBACK(ReloadLang), nullptr);
    gtk_box_append(LangButBox, GTK_WIDGET(reloadbutton));

    LoadLanguages();
}

void style::OpenLangChooser(GtkButton *self, EditArea *parent){
    EditAreaToChongeLang = parent;
    gtk_widget_set_visible(GTK_WIDGET(LangChoosingWindow), true);
}
