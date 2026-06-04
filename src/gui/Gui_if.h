#ifndef GUI_IF_H_
#define GUI_IF_H_

class MainWindow;
class FilePanel;
class SettingPanel;

namespace gui{

void OpenSettingPanel();
void Init();
void Start();

MainWindow* GetMainWindow();
FilePanel* GetFilePanel();
SettingPanel* GetSettingPanel();

}

 #endif
