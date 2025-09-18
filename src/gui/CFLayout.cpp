#include "CFLayout.h"
#include <glib/gprintf.h>
#include <utility>
#include <vector>
#include <memory>

static std::vector<std::unique_ptr<CFSeparator>> CacheSeparators;//For callback

static void SeparatorDragged(GtkGestureDrag* self, gdouble x, gdouble y, CFSeparator* cfsep){
    GtkWidget *previouswidget = nullptr, *nextwidget = nullptr;
    for (unsigned int i = 0; i < cfsep->parent->Widgets.size(); i++) {
        if (cfsep->parent->Widgets[i] == GTK_WIDGET(cfsep->separator)){
            previouswidget = cfsep->parent->Widgets[i-1];
            nextwidget = cfsep->parent->Widgets[i+1];
            break;
        }
    }

    if(previouswidget == nullptr || nextwidget == nullptr){
        return;
    }

    if(cfsep->parent->Orientation == GTK_ORIENTATION_HORIZONTAL){
        gtk_widget_set_size_request(previouswidget,
                gtk_widget_get_width(previouswidget) + x,
                gtk_widget_get_height(previouswidget));

        gtk_widget_set_size_request(nextwidget,
                gtk_widget_get_width(nextwidget) - x,
                gtk_widget_get_height(nextwidget));
    }else{
        gtk_widget_set_size_request(previouswidget,
                gtk_widget_get_width(previouswidget),
                gtk_widget_get_height(previouswidget) + y);

        gtk_widget_set_size_request(nextwidget,
                gtk_widget_get_width(nextwidget),
                gtk_widget_get_height(nextwidget) - y);
    }

}

CFLayout::CFLayout(GtkOrientation orientation){
    BaseBox = GTK_BOX(gtk_box_new(orientation, 0));
    Orientation = orientation;
}


void CFLayout::InsertChild(GtkWidget *child){
    if(!Widgets.empty()){
        GtkSeparator* sep = NewSeparator();
        gtk_box_append(BaseBox, GTK_WIDGET(sep));
    }

    Widgets.push_back(child);
    gtk_box_append(BaseBox, child);
}

GtkSeparator *CFLayout::NewSeparator(){
    CacheSeparators.emplace_back(std::make_unique<CFSeparator>());
    CFSeparator *cfsep = CacheSeparators.back().get();
    GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator

    cfsep->separator = GTK_SEPARATOR(gtk_separator_new(Orientation));
    cfsep->parent = this;
    gtk_widget_add_css_class(GTK_WIDGET(cfsep->separator), "Separator");
    gtk_widget_set_size_request(GTK_WIDGET(cfsep->separator), 5, 0);// height will be expanded
    gtk_widget_add_controller(GTK_WIDGET(cfsep->separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
    g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragged), cfsep);

    Widgets.push_back(GTK_WIDGET(cfsep->separator));
    return cfsep->separator;
}
