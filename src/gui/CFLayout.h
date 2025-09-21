#ifndef CFGRID_H_
#define CFGRID_H_

#include <gtk/gtk.h>
#include <utility>
#include <vector>
#include <map>

class LayoutNeighbor;

/*
 * Usage
 *
 * constructing: layoutobject.Init(GTK_ORIENTATION_...);
 * add new child: layoutobject.InsertChild(GTK_WIDGET(widget));
 * remove child: layoutobject.ReomoveChild(GTK_WIDGET(widget));
 */
class CFLayout{
public:
    std::vector<GtkWidget*> Widgets;// from left to right or from top to bottom
    GtkBox *BaseBox;
    GtkOrientation Orientation;
    void Init(GtkOrientation orientation);
    void InsertChild(GtkWidget *child);
    void RemoveChild(GtkWidget *child);
    LayoutNeighbor GetNeighbors(GtkWidget *widget);
private:
    GtkSeparator *NewSeparator();
};

class LayoutNeighbor{
public:
    /*
     * This is for GetNeighbors functon
     *
     */
    int Prevpos = -1, Nextpos = -1;
    GtkWidget *PrevWid = nullptr, *NextWid = nullptr;
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
