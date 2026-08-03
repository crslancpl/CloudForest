#include "CfLayout.h"

#include "components/CfContent.h"

#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <vector>
#include <memory>

static void SeparatorDragged(GtkGestureDrag* self, gdouble x, gdouble y, CfSeparator* cfsep){
    LayoutNeighbor neighbors = cfsep->GetParentLayout().GetNeighbors(cfsep);

    if(neighbors.prevContent == nullptr || neighbors.prevContent == nullptr){
        return;
    }

    GtkWidget* prevw = neighbors.prevContent->GetBaseWidget();
    GtkWidget* nextw = neighbors.nextContent->GetBaseWidget();;
    if(gtk_widget_get_width(prevw) + x <= 0 || gtk_widget_get_width(nextw) - x <= 0){
        return;
    }

    if(cfsep->GetParentLayout().GetOrientation() == GTK_ORIENTATION_HORIZONTAL){
        gtk_widget_set_size_request(prevw,
                gtk_widget_get_width(prevw) + x,
                gtk_widget_get_height(prevw));

        gtk_widget_set_size_request(nextw,
                gtk_widget_get_width(nextw) -x,
                gtk_widget_get_height(nextw));

    }else{
        gtk_widget_set_size_request(prevw,
                gtk_widget_get_width(prevw),
                gtk_widget_get_height(prevw) + y);

        gtk_widget_set_size_request(nextw,
                gtk_widget_get_width(nextw),
                gtk_widget_get_height(nextw) - y);
    }
}



/*
 * CfSeparator
 */

CfSeparator::CfSeparator(CfLayout& parent, GtkOrientation orientation):
    m_parentLayout(parent){
    m_separator = GTK_SEPARATOR(gtk_separator_new(orientation));
    gtk_widget_add_css_class(GTK_WIDGET(m_separator), "separator");
    if(orientation == GTK_ORIENTATION_HORIZONTAL){
        gtk_widget_set_size_request(GTK_WIDGET(m_separator), 5, 0);// height will be expanded
    }else{
        gtk_widget_set_size_request(GTK_WIDGET(m_separator), 0, 5);// width will be expanded
    }
    GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator
    gtk_widget_add_controller(GTK_WIDGET(m_separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
    g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragged), this);
}


CfLayout &CfSeparator::GetParentLayout(){
    return m_parentLayout;
}

// override

GtkWidget* CfSeparator::GetBaseWidget(){
    return GTK_WIDGET(m_separator);
}




/*
 * CfLayout
 */

CfLayout::CfLayout(GtkOrientation orientation){
    m_baseBox = GTK_BOX(gtk_box_new(orientation, 0));
    m_orientation = orientation;
    gtk_widget_set_overflow(GTK_WIDGET(m_baseBox), GTK_OVERFLOW_HIDDEN);
    SetContentWidget(GTK_WIDGET(m_baseBox));
}

CfLayout::~CfLayout(){
    //
}

LayoutNeighbor CfLayout::GetNeighbors(CfContent *child){
    /*
     * If the widget is not inside the layout, the map
     * will be empty
     */

    LayoutNeighbor neighbor;// position, widget ptr
    int objpos = -1;

    for (const std::unique_ptr<CfContent>& item : m_childContents) {
        objpos ++;
        if (child == item.get()){
            if(objpos != 0){
                // First one
                neighbor.prevPos = objpos -1;
                neighbor.prevContent = m_childContents[objpos -1].get();
            }

            if(objpos != m_childContents.size() -1 ){
                // Last one
                neighbor.nextPos = objpos +1;
                neighbor.nextContent = m_childContents[objpos + 1].get();
            }
            break;
        }
    }

    return neighbor;
}

GtkOrientation CfLayout::GetOrientation(){
    return m_orientation;
}

void CfLayout::Insert(std::unique_ptr<CfContent> child){
    if(!m_childContents.empty()){
        std::unique_ptr<CfSeparator> cfsep = std::make_unique<CfSeparator>(*this, m_orientation);
        gtk_box_append(m_baseBox, GTK_WIDGET(cfsep->GetBaseWidget()));
        m_childContents.push_back(std::move(cfsep));
    }
    gtk_widget_set_overflow(GTK_WIDGET(child->GetBaseWidget()), GTK_OVERFLOW_HIDDEN);
    gtk_box_append(m_baseBox, child->GetBaseWidget());
    m_childContents.push_back(std::move(child));
}


void CfLayout::Remove(CfContent *child){
    /*
     * Warning: Don't remove separator manually.
     *
     * The widgets surrounding the child should be one
     * or two separators
     */

    LayoutNeighbor neighbor = GetNeighbors(child);
    if (neighbor.prevContent != nullptr) {
        // If the "child" is in the middle or last one in
        // the layout, remove the separator before "child".
        m_childContents.erase(m_childContents.begin() + neighbor.prevPos);
        gtk_box_remove(m_baseBox, GTK_WIDGET(neighbor.prevContent));
        // Since the separator before "child" is erased, "child"
        // is moved forward to the positon that was the separator.
        m_childContents.erase(m_childContents.begin() + neighbor.prevPos);
        gtk_box_remove(m_baseBox, GTK_WIDGET(child));
    }else if(neighbor.nextContent != nullptr){
        // If the "child" is the first child in the layout,
        // remove the separator after it.
        m_childContents.erase(m_childContents.begin() + neighbor.nextPos);
        gtk_box_remove(m_baseBox, GTK_WIDGET(neighbor.nextContent));
        // The position of the "child" is still the first one
        m_childContents.erase(m_childContents.begin());
        gtk_box_remove(m_baseBox, GTK_WIDGET(child));
    }
}
