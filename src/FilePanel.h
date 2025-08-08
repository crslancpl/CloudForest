#pragma once
#include <gtk/gtk.h>
#include <memory>
#include "Classes.h"

/* File panel file button */
class FPFileButton {
public:
    GFile *file;
    shared_ptr<EditArea> ea;
    GtkButton *Button;
    char *FileAbsoPath;
    char *FileRelePath;
    char *FileName;

    void init(GFile *FileGFile,int level);
    void Open();
};

class FilePanel{
public:
    static int OffSet;
    void init();
    GtkGrid *BaseGrid;
    GtkBox *FileTree;
    FPFolderButton& NewFolder(GFile *file,GFile *ParentFolder,FPFolderButton& Parent);// Not nullable. The root folder will not be added with this function
    FPFileButton& NewFile(GFile *file, FPFolderButton& Parent);// Not nullable. File has to be under a folder
    void SetParent(GFile *file);
};

class FPFolderButton {
public:
    bool ChildLoaded = false;
    GtkBuilder *builder;
    bool Inited = false;
    char *FolderName;
    GtkBox *BaseBox;
    GtkButton *FolderToggleBut;
    GtkBox *Content;
    GFile *f;
    void init(GFile &Folder,GFile *Parent,int level);
    void AddChildFolder(FPFolderButton& Child);
    void AddChildFile(FPFileButton& Child);
    void SetAsRoot(GtkBox *Box);
    int Level;

    bool IsOpen=false;
};

void ToggleFolder(GtkButton* self,FPFolderButton *F);
void FileButtonClick(GtkButton *self,FPFileButton &Parent);
