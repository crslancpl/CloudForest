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
class FilePanelMenu;
class FPFolderMenu;
class FPFileMenu;

typedef struct FileData FileData;
typedef struct AppContext AppContext;

/*
 * FilePanelButton base class
 */

class FilePanelButton : public CfComponent {
public:
    FilePanelButton(Branch& branch, unsigned int level, AppContext& context);
    ~FilePanelButton();

    void LoadData();
    void ShowMenu();

    bool GetIsEditing();
    GtkButton* GetButton();

    virtual void Rename();
    virtual void RenameEnded();
    virtual void Delete();

protected:
    AppContext& m_context;

    GtkButton *m_button;
    GtkEditableLabel *m_nameEditLabel;
    GtkLabel *m_nameLabel;
    GtkImage *m_icon;
    GtkGestureClick *m_rightClickGesture;

    std::unique_ptr<FilePanelMenu> m_menu;

    unsigned int m_level;// Root folder is 0

private:
    Branch& m_branch;

    GtkBox *m_box;
};

/* File panel folder button */
class FPFolderButton : public FilePanelButton {
public:
    FPFolderButton(FolderBranch &folderbranch, int level, AppContext& context);
    ~FPFolderButton();

    void AddChildFolder(std::unique_ptr<FPFolderButton> child);
    void RemoveChildFolder(FPFolderButton* child);
    void AddChildFile(std::unique_ptr<FPFileButton> child);
    void RemoveChildFile(FPFileButton* child);

    void ToggleFolder();

    unsigned int GetLevel();

    GtkWidget* GetBaseWidget() override;

private:
    FolderBranch& m_folderBranch;

    std::vector<std::unique_ptr<FPFileButton>> m_childFiles;
    std::vector<std::unique_ptr<FPFolderButton>> m_childFolders;

    GtkBox *m_baseBox;// Containing FolderToggleBut and Content

    GtkBox *m_folderArea;// sub folder
    GtkBox *m_fileArea;// child file

    bool m_isOpen = false;
    bool m_childLoaded = false;
};



/* File panel file button */
class FPFileButton : public FilePanelButton {
public:
    FPFileButton(FileBranch &filebranch, int level, AppContext& context);// the level of Root folder is 0
    ~FPFileButton();

    void Clicked();

    GtkWidget* GetBaseWidget() override;

private:
    FileBranch& m_fileBranch;
};

#endif
