#ifndef FILEPANEL_H_
#define FILEPANEL_H_
#include <gtk/gtk.h>
#include <memory>
#include <vector>
#include <string>
#include "../Classes.h"


class FilePanel{
public:
    std::vector<std::shared_ptr<FPFolderButton>> FolderButtons;
    std::vector<std::shared_ptr<FPFileButton>> FileButtons;
    static unsigned short Offset;
    // Space in front of the buttons in FilePanel. OffSet * level is the total space

    GtkBuilder *builder;
    GtkGrid *BaseGrid;
    GtkBox *FileTree;

    void init();
    void UnrefBuilder();
    void LoadRoot(GFile *file, GFileInfo *fileinfo);
    FPFolderButton* FindFolderBut_GFile(GFile *folder);
    FPFileButton* FindFileBut_GFile(GFile *file);
    FPFolderButton& NewFolder(GFile *file,GFile *parentfolder,FPFolderButton* parentfolderbut);
    FPFileButton& NewFile(GFile *file, FPFolderButton* parentfolderbut);
private:
    void AddNewRoot(FPFolderButton& folderbutton);
    void AddNewRoot(FPFileButton& filebutton);
    // Not nullable. File has to be under a folder
};


/* File panel folder button */
class FPFolderButton {
public:
    ~FPFolderButton();
    GtkBuilder *builder;// unref by UnrefBuilder();
    GtkBox *BaseBox;// Containing FolderToggleBut and Content
    GtkButton *FolderToggleBut;
    GtkBox *Content;// Containing child files and child folders
    GFile *Folder;// Corresponding GFile

    std::string FolderName;
    std::string FolderPath;

    int Level;// Root folder is 0

    bool IsOpened=false;
    bool ChildLoaded = false;
    // To save memory, child files and folders will only be
    // loaded when the Content got expanded for the first time.


    void init(GFile *folder,GFile *parentfolder,int level);
    void AddChildFolder(FPFolderButton& Child);
    void AddChildFile(FPFileButton& Child);
    void UnrefBuilder();
};



/* File panel file button */
class FPFileButton {
public:
    ~FPFileButton();

    GFile *File;
    GtkButton *Button;

    std::string FileAbsoPath;
    std::string FileRelePath;
    std::string FileName;

    void init(GFile *filegfile,int level);// the level of Root folder is 0
    void Open();
};


#endif
