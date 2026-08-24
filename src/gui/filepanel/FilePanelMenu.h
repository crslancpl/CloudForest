#ifndef FILEPANELMENU_H_
#define FILEPANELMENU_H_

#include "filepanel/FilePanelButtons.h"
#include <gtk/gtk.h>

class FilePanelMenu {
public:
    FilePanelMenu(FilePanelButton& button);
    ~FilePanelMenu();

    void Show();
    void Hide();

protected:
    GtkPopoverMenu *m_popover;
    GtkBox *m_box;
    GMenu *m_gMenu;

    FilePanelButton& m_button;
};

class FPFileMenu : public FilePanelMenu {
public:
    FPFileMenu(FPFileButton& button);
    ~FPFileMenu();
};

class FPFolderMenu : public FilePanelMenu {
public:
    FPFolderMenu(FPFolderButton& button);
    ~FPFolderMenu();

private:
    GMenu *m_folderGMenu;
};

#endif
