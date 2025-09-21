#include "CFLayout.h"
#include <glib/gprintf.h>
#include <map>
#include <utility>
#include <vector>
#include <memory>

static std::vector<std::unique_ptr<CFSeparator>> CacheSeparators;//For callback

static void SeparatorDragged(GtkGestureDrag* self, gdouble x, gdouble y, CFSeparator* cfsep){
    LayoutNeighbor neighbors = cfsep->parent->GetNeighbors(GTK_WIDGET(cfsep->separator));

    if(neighbors.PrevWid == nullptr || neighbors.NextWid == nullptr){
        return;
    }

    if(cfsep->parent->Orientation == GTK_ORIENTATION_HORIZONTAL){
        gtk_widget_set_size_request(neighbors.PrevWid,
                gtk_widget_get_width(neighbors.PrevWid) + x,
                gtk_widget_get_height(neighbors.PrevWid));

        gtk_widget_set_size_request(neighbors.NextWid,
                gtk_widget_get_width(neighbors.NextWid) - x,
                gtk_widget_get_height(neighbors.NextWid));
    }else{
        gtk_widget_set_size_request(neighbors.PrevWid,
                gtk_widget_get_width(neighbors.PrevWid),
                gtk_widget_get_height(neighbors.PrevWid) + y);

        gtk_widget_set_size_request(neighbors.NextWid,
                gtk_widget_get_width(neighbors.NextWid),
                gtk_widget_get_height(neighbors.NextWid) - y);
    }

}

void CFLayout::Init(GtkOrientation orientation){
    BaseBox = GTK_BOX(gtk_box_new(orientation, 0));
    Orientation = orientation;
}

LayoutNeighbor CFLayout::GetNeighbors(GtkWidget* widget){
    /*
     * If the widget is not inside the layout, the map
     * will be empty
     */

    LayoutNeighbor neighbor;// position, widget ptr
    int objpos = -1;

    for (GtkWidget* wid : Widgets) {
        objpos ++;
        if (wid == widget){
            if(objpos != 0){
                // First one
                neighbor.Prevpos = objpos -1;
                neighbor.PrevWid = Widgets[objpos -1];
            }

            if(objpos != Widgets.size() -1 ){
                // Last one
                neighbor.Nextpos = objpos +1;
                neighbor.NextWid = Widgets[objpos + 1];
            }
            break;
        }
    }

    return neighbor;
}

void CFLayout::InsertChild(GtkWidget *child){
    if(!Widgets.empty()){
        GtkSeparator* sep = NewSeparator();
        gtk_box_append(BaseBox, GTK_WIDGET(sep));
    }

    Widgets.push_back(child);
    gtk_box_append(BaseBox, child);
}

void CFLayout::RemoveChild(GtkWidget *child){
    /*
     * Warning: Don't remove separator manually.
     *
     * The widgets surrounding the child should be one
     * or two separators
     */

    LayoutNeighbor neighbor = GetNeighbors(child);
    if (neighbor.PrevWid != nullptr) {
        // If the "child" is in the middle or last one in
        // the layout, remove the separator before "child".
        Widgets.erase(Widgets.begin() + neighbor.Prevpos);
        gtk_box_remove(BaseBox, GTK_WIDGET(neighbor.PrevWid));
        // Since the separator before "child" is erased, "child"
        // is moved forward to the positon that was the separator.
        Widgets.erase(Widgets.begin() + neighbor.Prevpos);
        gtk_box_remove(BaseBox, GTK_WIDGET(child));
    }else if(neighbor.NextWid != nullptr){
        // If the "child" is the first child in the layout,
        // remove the separator after it.
        Widgets.erase(Widgets.begin() + neighbor.Nextpos);
        gtk_box_remove(BaseBox, GTK_WIDGET(neighbor.NextWid));
        // The position of the "child" is still the first one
        Widgets.erase(Widgets.begin());
        gtk_box_remove(BaseBox, GTK_WIDGET(child));
    }
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
