#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_
#pragma once

#include <gtk/gtk.h>

#include "Classes.h"

class MainWindow{
public:
    EditAreaHolder *EAHolder;
    GtkWindow *Window;
    FilePanel *FP;
    HeaderBar *Headerbar;
    GtkGrid *WindowGrid;
};

void NewWindow (GtkApplication *app,gpointer user_data);

#endif
