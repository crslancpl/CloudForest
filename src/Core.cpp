#include "Core.h"
#include "Classes.h"
#include "EditArea.h"
#include "FilePanel.h"
#include <memory>
#include <vector>
#include <algorithm>

vector<shared_ptr<EditArea>> AllEditArea;
vector<shared_ptr<FPFolderButton>> AllFolderButtons;
vector<shared_ptr<FPFileButton>> AllFileButtons;

int EditAreaNum;
MainWindow AppWindow;


MainWindow &GetAppWindow(){
    return AppWindow;
}

shared_ptr<FPFileButton> NewFileButton(){
    AllFileButtons.emplace_back(make_shared<FPFileButton>());
    shared_ptr<FPFileButton> f = AllFileButtons[AllFileButtons.size() -1];
    return f;
}

shared_ptr<FPFolderButton> NewFolderButton(){
    AllFolderButtons.emplace_back(make_shared<FPFolderButton>());
    shared_ptr<FPFolderButton> p = AllFolderButtons[AllFolderButtons.size() -1];
    return p;// new Folder will by pushed to the last position
}

shared_ptr<EditArea>& NewEditArea(GFile* filetoedit, FPFileButton* filebutton){
    /*
     * Create a edit area in AllEditArea and return the reference
     */
    AllEditArea.emplace_back(make_shared<EditArea>(filetoedit, filebutton));
    return AllEditArea[AllEditArea.size()-1];
}

void AddEditArea(shared_ptr<EditArea> &EditAreaPtr){
    AllEditArea.push_back(EditAreaPtr);
    //g_print(" Editarea count %d\n", (int)AllEditArea.size());
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

shared_ptr<EditArea> GetEditAreaFromFileAbsoPath(const string &AbsoPath){
    for(shared_ptr<EditArea> ea: AllEditArea){
        if(ea->AbsoPath == AbsoPath){
            return ea;
        }
    }
    //g_print("No edit area found with this file name ");
    return NULL;
}
