#include "Style.h"
#include "guiCore.h"
#include "../ToolFunctions.h"



void style::LoadCssFromPath(const std::string &path){
    GtkCssProvider *CssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(CssProvider, path.c_str());
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(CssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}


void style::LoadTextTag(GtkTextBuffer *buffer){
    gtk_text_buffer_create_tag(buffer, "type", "foreground","rgb(165,229,112)", nullptr);
    gtk_text_buffer_create_tag(buffer, "keyword", "foreground","cyan", nullptr);
    gtk_text_buffer_create_tag(buffer, "none", "foreground","white", nullptr);
    gtk_text_buffer_create_tag(buffer, "cmt", "foreground","rgb(58,193,114)", nullptr);
    gtk_text_buffer_create_tag(buffer, "scmt", "foreground","orange", nullptr);
    gtk_text_buffer_create_tag(buffer, "text", "foreground","magenta", nullptr);
    gtk_text_buffer_create_tag(buffer, "tag", "foreground","rgb(176,112,229)", nullptr);
    gtk_text_buffer_create_tag(buffer, "func", "foreground","rgb(199,176,252)", nullptr);
    gtk_text_buffer_create_tag(buffer, "value", "foreground","rgb(128,188,237)", nullptr);
}



/*
 * Language choosing panel
 */

static GtkWindow *LangChoosingWindow = nullptr;
static GtkBox *box = nullptr;
static EditArea *EditAreaToChongeLang;//

static void LangChoosen(GtkButton* self, gpointer data){
    EditAreaToChongeLang ->ChangeLanguage(gtk_button_get_label(self),true);
}

static void LoadLanguages(){
    LangChoosingWindow = GTK_WINDOW(gtk_window_new());
    gtk_window_set_hide_on_close(LangChoosingWindow, true);
    gtk_window_set_transient_for(LangChoosingWindow, gui::AppWindow.Window);
    gtk_window_set_title(GTK_WINDOW(LangChoosingWindow), "Choose a language");
    gtk_window_set_default_size(LangChoosingWindow, 200, 100);

    box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2));
    gtk_window_set_child(LangChoosingWindow, GTK_WIDGET(box));


    GFile *folder = g_file_new_for_path("syntax");
    GFileEnumerator *childenum = g_file_enumerate_children(folder, "standard::name, standard::type",
        GFileQueryInfoFlags::G_FILE_QUERY_INFO_NONE, nullptr, nullptr);

    GFileInfo *info;
    while ((info = g_file_enumerator_next_file(childenum, nullptr, nullptr)) != nullptr) {
        if (g_file_info_get_file_type(info) == G_FILE_TYPE_DIRECTORY) continue;
        GFile *child = g_file_enumerator_get_child(childenum, info);
        if (child == nullptr ) continue;

        vector<string> d = TrimText(g_file_info_get_name(info), ".");

        GtkButton *newlangbut = GTK_BUTTON(gtk_button_new_with_label(d[0].c_str()));
        gtk_box_append(box, GTK_WIDGET(newlangbut));

        gtk_widget_add_css_class(GTK_WIDGET(newlangbut), "normalbutton");

        g_signal_connect(newlangbut, "clicked", G_CALLBACK(LangChoosen), nullptr);

        g_object_unref(info);
    };
}

void style::OpenLangChooser(GtkButton *self, EditArea *parent){
    if(LangChoosingWindow == nullptr){
        LoadLanguages();
    }

    EditAreaToChongeLang = parent;
    gtk_widget_set_visible(GTK_WIDGET(LangChoosingWindow), true);
}
