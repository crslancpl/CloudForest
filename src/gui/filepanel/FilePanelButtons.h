#ifndef FILEPANELBUTTONS_H_
#define FILEPANELBUTTONS_H_

#include <gtk/gtk.h>


class FPFolderButton;
class FPFileButton;

typedef struct FileData FileData;


class FPButtons{
    FileData *m_fildData;
};

/* File panel folder button */
class FPFolderButton {
public:
    FPFolderButton(GFile *folder, int level);
    ~FPFolderButton();
    FileData *m_fileData;

    GtkBuilder *builder;// unref by UnrefBuilder();
    GtkBox *m_baseBox;// Containing FolderToggleBut and Content
    GtkButton *m_folderToggleBut;
    GtkBox *m_content;// Containing child files and child folders

    int Level;// Root folder is 0
    bool IsOpened=false;
    bool ChildLoaded = false;

    void AddChildFolder(FPFolderButton* Child);
    void AddChildFile(FPFileButton* Child);
    void UnrefBuilder();
};



/* File panel file button */
class FPFileButton {
public:
    FPFileButton(GFile *file, int level);// the level of Root folder is 0
    ~FPFileButton();

    FileData *m_fileData;
    GtkButton *m_button;

    void Open();
};

#endif
