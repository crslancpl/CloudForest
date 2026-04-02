#include "CfLayout.h"

#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <vector>
#include <memory>

static std::vector<std::unique_ptr<CfSeparator>> cache_separators;//For callback

static void SeparatorDragged(GtkGestureDrag* self, gdouble x, gdouble y, CfSeparator* cfsep){
    LayoutNeighbor neighbors = cfsep->parent->GetNeighbors(GTK_WIDGET(cfsep->separator));

    if(neighbors.prevWid == nullptr || neighbors.nextWid == nullptr){
        return;
    }

    if(gtk_widget_get_width(neighbors.prevWid) + x <= 0 && gtk_widget_get_width(neighbors.nextWid) - x <= 0){
        return;
    }

    if(cfsep->parent->m_orientation == GTK_ORIENTATION_HORIZONTAL){
        gtk_widget_set_size_request(neighbors.prevWid,
                gtk_widget_get_width(neighbors.prevWid) + x,
                gtk_widget_get_height(neighbors.prevWid));

        gtk_widget_set_size_request(neighbors.nextWid,
                gtk_widget_get_width(neighbors.nextWid) -x,
                gtk_widget_get_height(neighbors.nextWid));

    }else{
        gtk_widget_set_size_request(neighbors.prevWid,
                gtk_widget_get_width(neighbors.prevWid),
                gtk_widget_get_height(neighbors.prevWid) + y);

        gtk_widget_set_size_request(neighbors.nextWid,
                gtk_widget_get_width(neighbors.nextWid),
                gtk_widget_get_height(neighbors.nextWid) - y);
    }

}

CfLayout::CfLayout(GtkOrientation orientation){
    m_baseBox = GTK_BOX(gtk_box_new(orientation, 0));
    m_orientation = orientation;
    gtk_widget_set_overflow(GTK_WIDGET(m_baseBox), GTK_OVERFLOW_HIDDEN);
    SetContentWidget(GTK_WIDGET(m_baseBox));
}

LayoutNeighbor CfLayout::GetNeighbors(GtkWidget* widget){
    /*
     * If the widget is not inside the layout, the map
     * will be empty
     */

    LayoutNeighbor neighbor;// position, widget ptr
    int objpos = -1;

    for (GtkWidget* wid : m_childWidgets) {
        objpos ++;
        if (wid == widget){
            if(objpos != 0){
                // First one
                neighbor.prevPos = objpos -1;
                neighbor.prevWid = m_childWidgets[objpos -1];
            }

            if(objpos != m_childWidgets.size() -1 ){
                // Last one
                neighbor.nextPos = objpos +1;
                neighbor.nextWid = m_childWidgets[objpos + 1];
            }
            break;
        }
    }

    return neighbor;
}

void CfLayout::InsertChild(CfContent *child){
    if(!m_childWidgets.empty()){
        GtkSeparator* sep = NewSeparator();
        gtk_box_append(m_baseBox, GTK_WIDGET(sep));
    }
    gtk_widget_set_overflow(GTK_WIDGET(child->GetBaseWidget()), GTK_OVERFLOW_HIDDEN);
    m_childWidgets.push_back(child->GetBaseWidget());
    gtk_box_append(m_baseBox, child->GetBaseWidget());
}


void CfLayout::RemoveChild(GtkWidget *child){
    /*
     * Warning: Don't remove separator manually.
     *
     * The widgets surrounding the child should be one
     * or two separators
     */

    LayoutNeighbor neighbor = GetNeighbors(child);
    if (neighbor.prevWid != nullptr) {
        // If the "child" is in the middle or last one in
        // the layout, remove the separator before "child".
        m_childWidgets.erase(m_childWidgets.begin() + neighbor.prevPos);
        gtk_box_remove(m_baseBox, GTK_WIDGET(neighbor.prevWid));
        // Since the separator before "child" is erased, "child"
        // is moved forward to the positon that was the separator.
        m_childWidgets.erase(m_childWidgets.begin() + neighbor.prevPos);
        gtk_box_remove(m_baseBox, GTK_WIDGET(child));
    }else if(neighbor.nextWid != nullptr){
        // If the "child" is the first child in the layout,
        // remove the separator after it.
        m_childWidgets.erase(m_childWidgets.begin() + neighbor.nextPos);
        gtk_box_remove(m_baseBox, GTK_WIDGET(neighbor.nextWid));
        // The position of the "child" is still the first one
        m_childWidgets.erase(m_childWidgets.begin());
        gtk_box_remove(m_baseBox, GTK_WIDGET(child));
    }
}

GtkSeparator *CfLayout::NewSeparator(){
    cache_separators.emplace_back(std::make_unique<CfSeparator>());
    CfSeparator *cfsep = cache_separators.back().get();
    GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator

    cfsep->separator = GTK_SEPARATOR(gtk_separator_new(m_orientation));
    cfsep->parent = this;

    gtk_widget_add_css_class(GTK_WIDGET(cfsep->separator), "Separator");

    if(m_orientation == GTK_ORIENTATION_HORIZONTAL){
        gtk_widget_set_size_request(GTK_WIDGET(cfsep->separator), 5, 0);// height will be expanded
    }else{
        gtk_widget_set_size_request(GTK_WIDGET(cfsep->separator), 0, 5);// width will be expanded
    }

    gtk_widget_add_controller(GTK_WIDGET(cfsep->separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
    g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragged), cfsep);

    m_childWidgets.push_back(GTK_WIDGET(cfsep->separator));
    return cfsep->separator;
}
