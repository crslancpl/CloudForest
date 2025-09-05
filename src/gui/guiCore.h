/*
 * Some codes is only applied within gui
 */

#ifndef GUICORE_H_
#define GUICORE_H_

//#include "FilePanel.h"
#include "FilePanel.h"
#include "HeaderBar.h"
#include "MainWindow.h"
#include "SettingPanel.h"
#include <gtk/gtk.h>

namespace gui{
extern MainWindow AppWindow;
extern HeaderBar AppHeaderBar;
extern SettingPanel AppSettingPanel;
extern FilePanel AppFilePanel;
void Init();
void OpenSettingPanel();
void OpenLangChooser(EditArea *parent);
void OpenFileChooser(bool fileordir, void (*callback)(GFile*,GFileInfo*));
void EnumFolder(GFile *folder, void (*callback)(GFile*,GFileInfo*));
}

 #endif
