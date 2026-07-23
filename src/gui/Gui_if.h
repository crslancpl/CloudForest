#ifndef GUI_IF_H_
#define GUI_IF_H_

class MainWindow;
class FilePanel;
class SettingPanel;
class DiagnosticPanel;
class LangPanel;
class CompletionPopover;
class CompletionTool;

typedef struct App App;

namespace gui{

int RunApp(int argc, char* argv[], App& app);// start the gtk application

MainWindow* GetMainWindow();
FilePanel* GetFilePanel();
SettingPanel* GetSettingPanel();
DiagnosticPanel* GetDiagnosticPanel();
LangPanel* GetLangPanel();
void TransferCompletionPopover(CompletionTool* newowner);

}// namespace gui

 #endif
