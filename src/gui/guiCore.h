/*
 * Some codes is only applied within gui
 */

#ifndef GUICORE_H_
#define GUICORE_H_

#include "../Core.h"
#include "FilePanel.h"
#include "HeaderBar.h"
#include "MainWindow.h"
#include "SettingPanel.h"
#include <gtk/gtk.h>
#include <string>

namespace gui{
extern MainWindow AppWindow;
extern HeaderBar AppHeaderBar;
extern SettingPanel AppSettingPanel;
extern FilePanel AppFilePanel;

void Init();
void Process(GUIAction *action);//for core

void cfLoadLanguage(const std::string& langname);
void cfProcessFile(const std::string& filepath, const std::string& language);

//async
void OpenFileChooser(bool fileordir, void (*callback)(GFile*,GFileInfo*));
void SaveFile(GFile* file, char* content, void(*callback)(GFile*,GFileInfo*));
void EnumFolder(GFile *folder, void (*callback)(GFile*,GFileInfo*));
void GetEditAreaContent(const std::string &filepath, void (*callback)(char*));
}

 #endif
