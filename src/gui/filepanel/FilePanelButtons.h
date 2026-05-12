#ifndef FILEPANELBUTTONS_H_
#define FILEPANELBUTTONS_H_

#include "datatypes/file.h"
#include "src/gui/components/CfComponent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>


class FPFolderButton;
class FPFileButton;

typedef struct FileData FileData;


/* File panel folder button */
class FPFolderButton : public CfComponent {
public:
    FPFolderButton(FileBranch *folderbranch, int level);
    ~FPFolderButton();


    void AddChildFolder(FPFolderButton* child);
    void AddChildFile(FPFileButton* child);
    void UnrefBuilder();
    void ToggleFolder();
    unsigned int GetLevel();

    GtkWidget* GetBaseWidget() override;

private:
    FileBranch* m_folderBranch;
    GtkBuilder *builder;// unref by UnrefBuilder();
    GtkBox *m_baseBox;// Containing FolderToggleBut and Content
    GtkButton *m_folderToggleBut;
    GtkBox *m_childArea;// containing folderArea and fileArea. Hide this when toggle
    GtkBox *m_folderArea;// sub folder
    GtkBox *m_fileArea;// child file

    unsigned int m_level;// Root folder is 0
    bool m_isOpen=false;
    bool m_childLoaded = false;
};



/* File panel file button */
class FPFileButton : public CfComponent {
public:
    FPFileButton(FileBranch *filebranch, int level);// the level of Root folder is 0
    ~FPFileButton();

    void Open();
    GtkWidget* GetBaseWidget() override;

private:
    FileBranch* m_fileBranch;
    GtkButton *m_button;
};

#endif
