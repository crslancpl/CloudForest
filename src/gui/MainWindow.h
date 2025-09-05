#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_
#pragma once

#include <gtk/gtk.h>

#include "../Classes.h"

class MainWindow{
public:
    void Init();
    void Show();
    void SetHeaderBar(GtkWidget *headerbar);
    GtkWindow *Window;
    FilePanel *FP;
    GtkGrid *WindowGrid;
    GtkSeparator *Separator;
};

#endif
