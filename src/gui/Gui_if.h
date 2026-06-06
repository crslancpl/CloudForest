#ifndef GUI_IF_H_
#define GUI_IF_H_

class MainWindow;
class FilePanel;
class SettingPanel;

namespace gui{

int RunApp(int argc, char* argv[]);// start the gtk application

MainWindow* GetMainWindow();
FilePanel* GetFilePanel();
SettingPanel* GetSettingPanel();

}// namespace gui

 #endif
