#ifndef FILEPANEL_IF_H_
#define FILEPANEL_IF_H_

class FPFolderButton;
typedef struct Workspace Workspace;
typedef struct FileData FileData;

namespace filepanel{

//void LoadWorkspace(Workspace *file);
void AddWorkspaceBox(Workspace* ws);
void Construct();
FPFolderButton* GetFolderButton(const FileData *folder);
}

#endif
