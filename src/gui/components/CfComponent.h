#ifndef CFCOMPONENT_H_
#define CFCOMPONENT_H_

#include <gtk/gtk.h>

class CfComponent{
public:
    virtual GtkWidget* GetBaseWidget() = 0;
};

#endif
