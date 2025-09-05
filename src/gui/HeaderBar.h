#ifndef HEADERBAR_H_
#define HEADERBAR_H_
#pragma once

#include <gtk/gtk.h>

#include "../Classes.h"

class HeaderBar{
public:
    void Init();
    GtkHeaderBar *HeaderBarWidget;
    GtkMenuButton *FileBut;
    // Other buttons is not funtioning yet
    GtkButton *CompileBut;
    GtkButton *IdeBut;
    GtkSearchEntry *SearchBar;
};


#endif
