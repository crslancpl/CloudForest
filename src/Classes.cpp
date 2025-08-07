#include "Classes.h"

#include <glib/gprintf.h>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <string.h>

vector<shared_ptr<EditArea>> SectionData::AllEditArea = {};
vector<shared_ptr<Folder>> SectionData::AllFolder = {};
vector<shared_ptr<File>> SectionData::AllFile ={};
MainWindow SectionData::currentwindow;

shared_ptr<File> SectionData::AddFile(){
    AllFile.emplace_back(make_shared<File>());
    shared_ptr<File> f = AllFile[AllFile.size() -1];
    return f;
}

shared_ptr<Folder> SectionData::AddFolder(){
    AllFolder.emplace_back(make_shared<Folder>());
    shared_ptr<Folder> p = AllFolder[AllFolder.size() -1];
    return p;// new Folder will by pushed to the last position
}

void SectionData::AddEditArea(shared_ptr<EditArea> EditAreaPtr){
    AllEditArea.push_back(EditAreaPtr);
    //g_print(" Editarea count %d\n", (int)AllEditArea.size());
}

void SectionData::RemoveEditArea(shared_ptr<EditArea> EditAreaPtr){
    int Pos = 0;
    for(shared_ptr<EditArea> ea: AllEditArea){
        if(ea->AbsoPath == EditAreaPtr->AbsoPath){
            gtk_widget_unparent(GTK_WIDGET(ea->BaseGrid));
            break;
        }
        Pos++;
    }
    AllEditArea.erase(AllEditArea.begin() + Pos);
}

shared_ptr<EditArea> SectionData::GetEditAreaFromFileAbsoPath(const string &AbsoPath){
    for(shared_ptr<EditArea> ea: AllEditArea){
        if(ea->AbsoPath == AbsoPath){
            g_print("Found");
            return ea;
        }
    }
    //g_print("No edit area found with this file name ");
    return NULL;
}

Suggestion::Suggestion(string *content, string *type, string *doc){
    Content = content;
    Type = type;
    Doc = doc;
}
