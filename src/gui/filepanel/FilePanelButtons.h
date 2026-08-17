#ifndef FILEPANELBUTTONS_H_
#define FILEPANELBUTTONS_H_

#include "src/filemanagement/FileTree.h"
#include "datatypes/file.h"
#include "components/CfComponent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <vector>


class FPFolderButton;
class FPFileButton;

typedef struct FileData FileData;


/* File panel folder button */
class FPFolderButton : public CfComponent {
public:
    FPFolderButton(FolderBranch &folderbranch, int level);
    ~FPFolderButton();

    void AddChildFolder(std::unique_ptr<FPFolderButton> child);
    void AddChildFile(std::unique_ptr<FPFileButton> child);
    void UnrefBuilder();
    void ToggleFolder();
    void ShowOptions();

    unsigned int GetLevel();

    GtkWidget* GetBaseWidget() override;

private:
    FolderBranch& m_folderBranch;
    std::vector<std::unique_ptr<FPFileButton>> m_childFiles;
    std::vector<std::unique_ptr<FPFolderButton>> m_childFolders;

    GtkBuilder *builder;// unref by UnrefBuilder();
    GtkBox *m_baseBox;// Containing FolderToggleBut and Content
    GtkButton *m_toggleButton;
    GtkBox *m_childArea;// containing folderArea and fileArea. Hide this when toggle
    GtkBox *m_folderArea;// sub folder
    GtkBox *m_fileArea;// child file
    GtkGestureClick *m_rightClickGesture;

    unsigned int m_level;// Root folder is 0
    bool m_isOpen=false;
    bool m_childLoaded = false;
};



/* File panel file button */
class FPFileButton : public CfComponent {
public:
    FPFileButton(FileBranch &filebranch, int level);// the level of Root folder is 0
    ~FPFileButton();

    void ShowOptions();

    void Clicked();

    GtkWidget* GetBaseWidget() override;

private:
    FileBranch& m_fileBranch;
    GtkButton *m_button;
    GtkGestureClick *m_mouseGesture;
};

#endif
