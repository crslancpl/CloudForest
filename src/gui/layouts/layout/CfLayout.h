#ifndef CFLAYOUT_H_
#define CFLAYOUT_H_

#include <gtk/gtk.h>
#include <vector>

#include "../../components/CfContent.h"

typedef struct{
    /*
     * This is for GetNeighbors functon
     */
    int prevPos, nextPos;
    GtkWidget *prevWid, *nextWid;
}LayoutNeighbor;

/*
 * Usage
 *
 * constructing: layoutobject.Init(GTK_ORIENTATION_...);
 * add new child: layoutobject.InsertChild(GTK_WIDGET(widget));
 * remove child: layoutobject.ReomoveChild(GTK_WIDGET(widget));
 */
class CfLayout : public CfContent{
public:
    GtkOrientation m_orientation;
    CfLayout(GtkOrientation orientation);
    void InsertChild(CfContent *child);
    void RemoveChild(GtkWidget *child);
    LayoutNeighbor GetNeighbors(GtkWidget *widget);
private:
    std::vector<GtkWidget*> m_childWidgets;// from left to right or from top to bottom

    GtkBox *m_baseBox;
    GtkSeparator *NewSeparator();
};



/*
 * This shouldn't be used outside CFLayout
 */
typedef struct {
    CfLayout *parent;
    GtkSeparator *separator;
}CfSeparator;

#endif
