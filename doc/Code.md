## Source code overview

All source codes are placed under `src` folder. 

## Classes

**All classes are forward defined in `Classes.h`**

>[!IMPORTANT]
>`Classes.h` is only for forward definition. `Classes.h` doesn't define any classes. Only use `Classes.h` in header files. Remember to include the header file that defines the class

### defined in MainWindow.h [[MainWindow]]

| Class      |                                 |
| ---------- | ------------------------------- |
| MainWindow | The class that holds the window |
### defined in HeaderBar.h [[HeaderBar]]

| Class     |                                |
| --------- | ------------------------------ |
| HeaderBar | The class that holds headerbar |
### defined in FilePanel.h [[FilePanel]]

| Class          |                                                   |
| -------------- | ------------------------------------------------- |
| FilePanel      | The FilePanel at the left hand side of the window |
| FPFolderButton | Folder button in FilePanel                        |
| FPFileButton   | File button in FilePanel                          |
### defined in EditArea.h [[EditArea]]

| Class                |                                                                                                                                 |
| -------------------- | ------------------------------------------------------------------------------------------------------------------------------- |
| EditArea             | The place users can edit the content                                                                                            |
| EditAreaHolder       | Contains tab switcher (EditAreaHolderTabBut) and the container for EditArea.                                                    |
| EditAreaHolderTabBut | The button that lets you switch between EditAreas                                                                               |
| Suggestions          | The information of the auto-complete/suggestions. Used by Language server, and EditArea will load the information then show it. |


## Workflow
### Open the app:
1. **Entry.cpp** creates the app and call `NewWindow()` in **MainWindow.cpp**.
2. `NewWindow()` in **MainWindow.cpp** store the App in **Core.cpp** with `SetApp()` and get the AppWindow object in **Core.cpp** with `GetAppWindow()`, then assign variables.
3. `NewWindow()` creates a  **FilePanel** and initialize it with `FilePanel->init()` method
4. `NewWindow()` creates an  **EditAreaHolder** and initialize it with `EditAreaHolder->init()` method
5. `NewWindow()` Construct **HeaderBar** with `LoadHeaderBar()` function
6. `NewWindow()` Sets the offset of FilePanel, load css, and other properties
7. `NewWindow()` Creates a new **EditArea** that doesn't open any file by calling `NewEditArea()` from **Core.cpp** with `nullptr` as parameter. Then show the EditArea in **EditAreaHolder**

### Open a folder:
1. Click on `File` button at the left-hand side of the headerbar. It will show a drop-down menu. Click **Open folder** and `LoadFolderClicked()`in **HeaderBar.cpp** will be triggered.
2. `LoadFolderClicked()` calls `OpenFileChooser()` with parameter `false`. Now a FileDialog is opened and wait for user to choose a folder.
3. After chosen, `FolderSelected()` will be called. And run `ReadAsRootFolder()`.
4. `ReadAsRootFolder()` creates a `FPFolderButton` by `NewFolderButton()` from **Core.cpp** and initialize it. Then call `FilePanel->AddNewRootFolder()` to show it as root folder
5. When user expand the content of the folder button by triggering `ToggleFolder()`, it will call `ReadFolder()` from **FileManager.cpp** and add FPFileButton/FPFolderButton under `GtkBox *Content`

### Open a file:
1. Click on `File` button at the left-hand side of the headerbar. It will show a drop-down menu. Click **Open file** and `LoadFileClicked()`in **HeaderBar.cpp** will be triggered.
2. `LoadFileClicked()` calls `OpenFileChooser()` with parameter `true`. Now a FileDialog is opened and wait for user to choose a file.
3. After chosen, `FileSelected()` will be called and run `OpenFile()`.

### Open a file in FilePanel:
1. Click on `FPFileButton` in the FilePanel. It will call `Open()` method which runs `OpenFile()`

>[!INFO]
>`OpenFile()` function from FileManager.cpp will create a EditArea and show it in EditAreaHolder

