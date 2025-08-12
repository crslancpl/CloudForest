#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_
#pragma once

#include <gtk/gtk.h>

#include "Classes.h"

class MainWindow{
public:
    EditAreaHolder *EAHolder;
    GtkApplication *App;
    GtkWindow *Window;
    FilePanel *FP;
    GtkButton *AppBut;

    GtkGrid *WindowGrid;
};

void NewWindow (GtkApplication *app,gpointer user_data);

#endif
