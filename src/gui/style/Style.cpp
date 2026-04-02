#include "Style.h"
#include <pango/pango-attributes.h>



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
