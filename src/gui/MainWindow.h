#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_
#pragma once

#include <gtk/gtk.h>
#include <memory>

#include "../Classes.h"
#include "CFLayout.h"

class MainWindow{
public:
    void Init();
    void Show();
    void SetHeaderBar(GtkWidget *headerbar);
    GtkWindow *Window;
    CFLayout Layout;
};

#endif
