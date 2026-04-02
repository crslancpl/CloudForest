#ifndef FILEPANEL_H_
#define FILEPANEL_H_
#include <gtk/gtk.h>

#include "src/gui/components/CfContent.h"
#include "FilePanelButtons.h"

typedef struct FileData FileData;

class FilePanel : public CfContent{
public:
    FilePanel();

    static unsigned short Offset;
    // Space in front of the buttons in FilePanel. OffSet * level is the total space

    void NewFolder(GFile *file,GFile *parentfolder,FPFolderButton* parentfolderbut);
    void NewFile(GFile *file, FPFolderButton* parentfolderbut);
    FPFolderButton* FindFolderBut_GFile(GFile *folder);
    FPFileButton* FindFileBut_GFile(GFile *file);

private:
    GtkBox *m_fileTree;

    void AddNewRoot(FPFolderButton* folderbutton);
    void AddNewRoot(FPFileButton* filebutton);
};

namespace filepanel{
    FPFolderButton *GetFileButton(FileData *filedata);
}

#endif
