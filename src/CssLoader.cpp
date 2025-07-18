#include <gtk/gtk.h>
#include <gtk/gtkcssprovider.h>
#include <gtk/gtkstyleprovider.h>
#include <string>

using namespace std;

void LoadCssFromPath(const string &path){
    GtkCssProvider *CssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(CssProvider, path.c_str());
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(CssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
