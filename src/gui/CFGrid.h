#ifndef CFGRID_H_
#define CFGRID_H_

#include <gtk/gtk.h>
#include <utility>
#include <vector>

class CFGridLayout{
public:
    bool vexpand;
    bool hexpand;
    int row;
    int col;
    int hspan;
    int vspan;
};

class CFGrid{
    GtkGrid *Grid;
    std::vector<std::pair<GtkWidget, CFGridLayout>> Children;
    void InsertChild(GtkWidget *child);
    void RemoveChild(GtkWidget *child);
};

#endif
