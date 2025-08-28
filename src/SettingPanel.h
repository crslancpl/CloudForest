#ifndef SETTINGPANEL_H_
#define SETTINGPANEL_H_

#include <gtk/gtk.h>
void InitSettingPanel();
void AddTabButton(GtkButton* button, void (*clickedcallback)(GtkButton*, gpointer));
void ShowSettingPanel();
void CloseSettingPanel();
void SwitchSettingTab(GtkButton *self, GtkBox* page);

#endif
