#ifndef FILEPANELBUTTONS_H_
#define FILEPANELBUTTONS_H_

#include "src/gui/components/CfComponent.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>


class FPFolderButton;
class FPFileButton;

typedef struct FileData FileData;


class FPButtons{
    FileData *m_fildData;
};

/* File panel folder button */
class FPFolderButton : public CfComponent {
public:
    FPFolderButton(GFile *folder, int level);
    ~FPFolderButton();


    void AddChildFolder(FPFolderButton* child);
    void AddChildFile(FPFileButton* child);
    void UnrefBuilder();
    void ToggleFolder();
    unsigned int GetLevel();

    GtkWidget* GetBaseWidget() override;

private:
    FileData *m_fileData;

    GtkBuilder *builder;// unref by UnrefBuilder();
    GtkBox *m_baseBox;// Containing FolderToggleBut and Content
    GtkButton *m_folderToggleBut;
    GtkBox *m_content;// Containing child files and child folders

    unsigned int m_level;// Root folder is 0
    bool m_isOpen=false;
    bool m_childLoaded = false;
};



/* File panel file button */
class FPFileButton : public CfComponent {
public:
    FPFileButton(GFile *file, int level);// the level of Root folder is 0
    ~FPFileButton();

    void Open();
    GtkWidget* GetBaseWidget() override;
private:
    FileData *m_fileData;
    GtkButton *m_button;
};

#endif
