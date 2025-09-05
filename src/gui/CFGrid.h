#ifndef CFGRID_H_
#define CFGRID_H_

#include <gtk/gtk.h>

class CFGrid{
    GtkGrid *Grid;
    void InsertChild(GtkWidget *child, bool expand, int row, int col, int hspan, int vspan);
    void RemoveChild(GtkWidget *child);
};

#endif
