/*
 * Core.h and Core.cpp is holding all EditAreas, FPFileButton, FPFolderButton, and other global datas in the app
 */

#ifndef CORE_H_
#define CORE_H_
#pragma once

#include "Classes.h"
#include "cf/CFEmbed.h"
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <memory>
#include <string>


using namespace std;

//CloudyForest callback function
void CfCallbackFunc(Message *message);
void InitCfEmbed();
void CfSendMessage(MessageType type, void* content);

//App
GtkApplication &GetApp();
void SetApp(GtkApplication* app);

//Window
MainWindow &GetAppWindow();

//FilePanel buttons
shared_ptr<FPFileButton> &NewFileButton();// The newly created file button will be pushed to AllFileButtons
shared_ptr<FPFolderButton> &NewFolderButton();// The newly created folder button will be pushed to AllFolderButtons
void RemoveFolder(FPFolderButton& folder);// Not working now

//EditArea
shared_ptr<EditArea>& NewEditArea(GFile* filetoedit, FPFileButton* filebutton);
void RemoveEditArea(EditArea* EditAreaPtr);
shared_ptr<EditArea>* GetEditAreaFromFileAbsoPath(const string &AbsPath);// Null if no EditArea for this file path exist
shared_ptr<EditArea>* GetEditAreaFromFileRelePath(const string &RelePath);
#endif
