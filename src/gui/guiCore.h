/*
 * Some codes is only applied within gui
 */

#ifndef GUICORE_H_
#define GUICORE_H_

#include "../Core.h"
#include "../Global.h"
#include "EditArea.h"
#include "FilePanel.h"
#include "HeaderBar.h"
#include "MainWindow.h"
#include "SettingPanel.h"

#include <future>
#include <gtk/gtk.h>
#include <string>

namespace gui{

//global variables
extern MainWindow AppWindow;
extern HeaderBar AppHeaderBar;
extern SettingPanel AppSettingPanel;
extern FilePanel AppFilePanel;
extern EditAreaHolder* FocusedEAHolder;//by default this will be pointing

void Init();

//
std::shared_ptr<EditArea>* NewEditArea(GFile *file);
std::shared_ptr<EditArea>* GetEditArea(const std::string &filename);
void RemoveEditArea(EditArea* editarea);
EditAreaHolder* GetEAHolder(int number);
EditAreaHolder* NewEAHolder();


//async callback
const result::Result* Process(request::Request* request);//for core
const result::Result* GetEditAreaContent(const std::string &filepath);
const result::Result* ChangeEditAreaLanguage(const std::string &filepath, const std::string &lang);

//async request
void OpenFileChooser(bool fileordir);
void SaveFile(GFile* file, char* content, void(*callback)(GFile*,GFileInfo*));
void EnumFolder(GFile *folder, void (*callback)(GFile*,GFileInfo*));

void cfLoadLanguage(const std::string& langname);
void cfProcessFile(const std::string& filepath, const std::string& language);

void PyRunCode(std::string& code);

}

 #endif
