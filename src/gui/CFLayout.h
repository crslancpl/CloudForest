#ifndef CFGRID_H_
#define CFGRID_H_

#include <gtk/gtk.h>
#include <utility>
#include <vector>

/*
 * Usage
 * constructing: CFLayout *layoutname = new CFLayout(GTK_ORIENTATION_...);
 * add new child: layoutname->InsertChild(GTK_WIDGET(widget));
 */
class CFLayout{
public:
    //static CFLayout* New(GtkOrientation orientation);
    std::vector<GtkWidget*> Widgets;// from left to right
    CFLayout(GtkOrientation orientation);
    GtkBox *BaseBox;
    GtkOrientation Orientation;
    void InsertChild(GtkWidget *child);
    void RemoveChild(GtkWidget *child);
private:
    GtkWidget *GetNeighbor(GtkSeparator *separator );
    GtkSeparator *NewSeparator();
};


/*
 * This shouldn't be used outside CFLayout
 */
class CFSeparator{
public:
    CFLayout *parent;
    GtkSeparator *separator;
};

#endif
