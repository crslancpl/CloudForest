#include "CFGrid.h"


static void SeparatorDragged(GtkGestureDrag* self, gdouble x, gdouble y, GtkGrid* grid){
    gtk_widget_set_size_request(GTK_WIDGET(core::GetMainWindow()->FP->BaseGrid),
            gtk_widget_get_width(GTK_WIDGET(core::GetMainWindow()->FP->BaseGrid)) + x, 0);
}

void CFGrid::InsertChild(GtkWidget *child){
    static int childcount = 0;
    if (childcount >=1) {
        GtkSeparator *separator = GTK_SEPARATOR(gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
        gtk_widget_add_css_class(GTK_WIDGET(separator), "Separator");
        gtk_widget_set_size_request(GTK_WIDGET(separator), 5, 0);// height will be expanded
        GtkGestureDrag *drag = GTK_GESTURE_DRAG(gtk_gesture_drag_new());// create drag gesture for separator
        gtk_widget_add_controller(GTK_WIDGET(separator), GTK_EVENT_CONTROLLER(drag));// connect drag and separator
        g_signal_connect(drag, "drag-update", G_CALLBACK(SeparatorDragged), Grid);
        gtk_grid_attach(Grid, GTK_WIDGET(separator), childcount,0, 1, 1);
        childcount++;
    }
    gtk_grid_attach(Grid, child, childcount, 0, 1, 1);
    childcount++;
}
