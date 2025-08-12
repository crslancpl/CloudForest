#include "Core.h"
#include "EditArea.h"
#include "FilePanel.h"
#include "MainWindow.h"

#include <memory>
#include <vector>
#include <algorithm>


vector<shared_ptr<EditArea>> AllEditArea;
// The shared_ptr<EditArea> will be shared in 2 to 3 places: here, FPFileButton(If in folder), and EditAreaHolderTabBut
vector<shared_ptr<FPFolderButton>> AllFolderButtons;
vector<shared_ptr<FPFileButton>> AllFileButtons;

int EditAreaNum;
MainWindow AppWindow;


MainWindow &GetAppWindow(){
    return AppWindow;
}

shared_ptr<FPFileButton>& NewFileButton(){
    /*
     * Create a file button in AllFileButton and return the reference
     */
    AllFileButtons.emplace_back(make_shared<FPFileButton>());
    return AllFileButtons[AllFileButtons.size() -1];
}

shared_ptr<FPFolderButton>& NewFolderButton(){
    /*
     * Create a folder button in AllFolderButton and return the reference
     */
    AllFolderButtons.emplace_back(make_shared<FPFolderButton>());
    return AllFolderButtons[AllFolderButtons.size() -1];// new Folder will by pushed to the last position
}

shared_ptr<EditArea>& NewEditArea(GFile* filetoedit, FPFileButton* filebutton){
    /*
     * Create a edit area in AllEditArea and return the reference
     */
    AllEditArea.emplace_back(make_shared<EditArea>(filetoedit, filebutton));
    return AllEditArea[AllEditArea.size()-1];
}

void RemoveEditArea(EditArea* EditAreaPtr){
    int Pos = 0;
    for(shared_ptr<EditArea> ea: AllEditArea){
        if(ea.get() == EditAreaPtr){
            gtk_widget_unparent(GTK_WIDGET(ea->BaseGrid));
            break;
        }
        Pos++;
    }
    AllEditArea.erase(AllEditArea.begin() + Pos);
}

shared_ptr<EditArea>* GetEditAreaFromFileAbsoPath(const string &AbsoPath){
    for(shared_ptr<EditArea>& ea: AllEditArea){
        if(ea->AbsoPath == AbsoPath){
            return &ea;
        }
    }
    return nullptr;
}
