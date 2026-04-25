/*
 * Some codes is only applied within gui
 */

#ifndef GUI_IF_H_
#define GUI_IF_H_

#include <gtk/gtk.h>

class MainWindow;
class HeaderBar;
class FilePanel;
class SettingPanel;

namespace gui{

extern MainWindow* g_mainwindow;
extern HeaderBar* g_headerbar;
extern FilePanel* g_filepanel;
extern SettingPanel* g_settingpanel;

void OpenSettingPanel();
void Init();
void Start();

}

 #endif
