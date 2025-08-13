Defined in `FilePanel.h` and `FilePanel.cpp`

![[Screenshot from 2025-08-13 10-35-52.png]]

## FilePanel class

```cpp
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
```

`Offset` indicates how backward will the child file/folder button be place compare to it's parent.

`BaseGrid` is the GtkWidget that is containing `FileTree`. The root folder's `BaseBox` is place inside `FileTree`. You can add many root folder in the file panel with `AddNewRootFolder()`.

To create a `FPFolderButton` or `FPFileButton`, use `NewFolder()` and `NewFile()`.

>[!IMPORTANT]
>We will move `NewFolder()` and `NewFile()` out of the class in the future.

## FPFolderButton class
```cpp
class FPFileButton {
public:
    GFile *file;
    shared_ptr<EditArea> ea;
    GtkButton *Button;
    char *FileAbsoPath;
    char *FileRelePath;
    char *FileName;

    void init(GFile *FileGFile,int level);// the level of Root folder is 0
    void Open();
};
```

**Use init to apply properties in the class.** 

`GtkButton *Button` is the button you see in the panel. If the button is clicked,  `FileButtonClick()`**(the function is not in the class, see Function section below.)** will be called, and run the `Open()` . 

## FPFileButton class

```cpp
class FPFolderButton {
public:
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
```

**Use init to apply properties in the class.** 

`GtkButton *FolderToggleBut` is the button you click to expand the `GtkBox *Content` in file panel. If the button is clicked,  `ToggleFolder()`**(the function is not in the class,  see Function section below.)** will be called, and expand `Content`.

To reduce memory usage, `FPFolderButton` will not load all of its child when you have selected a folder from the headerbar. It will only load children when you first expend the content.

### CSS:
```css
#FileTree button

#FilePanelBaseGrid

#FileTree scrolledwindow

.FolderButton

.FileButton

.rootfolder

```
## Function
```cpp
void ToggleFolder(GtkButton* self,FPFolderButton *F);
void FileButtonClick(GtkButton *self,FPFileButton &Parent);
```

**`ToggleFolder()` will call `ReadFolder()` from file `manager.cpp` if the content of `FPFolderButton *F` is not loaded.** And then expand the content of `FPFolderButton *F`.

`FileButtonClick()` is the callback function called when the folder button is clicked. What it does is call the `Open()` method of the second parameter `FPFileButton &Parent`

>[!WARNING]
>I know it is very inefficient to create a callback that only contains one line of code. It is now calling `open()` because I want to make the code understandable. This will be fixed in the future.

