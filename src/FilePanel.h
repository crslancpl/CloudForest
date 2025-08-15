#pragma once
#include <gtk/gtk.h>
#include <memory>
#include "Classes.h"

using namespace std;

class FilePanel{
public:
    static int OffSet;// Space in front of the buttons in FilePanel. OffSet * level is the total space
    GtkBuilder *builder;
    void init();
    GtkGrid *BaseGrid;
    GtkBox *FileTree;

    void AddNewRootFolder(FPFolderButton& folderbutton);

    FPFolderButton& NewFolder(GFile *file,GFile *ParentFolder,FPFolderButton& Parent);
    // Parent not nullable. The root folder will not be added with this function. See FPFolderButton::SetAsRoot()

    FPFileButton& NewFile(GFile *file, FPFolderButton& Parent);// Not nullable. File has to be under a folder
    void UnrefBuilder();
};

/* File panel file button */
class FPFileButton {
public:
    ~FPFileButton();
    GFile *file;
    shared_ptr<EditArea> ea;
    GtkButton *Button;
    char *FileAbsoPath;
    char *FileRelePath;
    char *FileName;

    void init(GFile *FileGFile,int level);// the level of Root folder is 0
    void Open();
};

/* File panel folder button */
class FPFolderButton {
public:
    ~FPFolderButton();
    GtkBuilder *builder;// unref by UnrefBuilder();
    bool ChildLoaded = false;
    // To save memory, child files and folders will only be loaded when the Content got expanded for the first time.
    bool Inited = false;
    char *FolderName;
    GtkBox *BaseBox;// Containing FolderToggleBut and Content
    GtkButton *FolderToggleBut;
    GtkBox *Content;// Containing child files and child folders
    GFile *Folder;// Corresponding GFile
    int Level;// Root folder is 0
    bool IsOpen=false;

    void init(GFile &folder,GFile *parentfolder,int level);
    void AddChildFolder(FPFolderButton& Child);
    void AddChildFile(FPFileButton& Child);
    void UnrefBuilder();
};

void ToggleFolder(GtkButton* self,FPFolderButton *F);
void FileButtonClick(GtkButton *self,FPFileButton &Parent);
