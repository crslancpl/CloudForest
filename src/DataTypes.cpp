#include "DataTypes.h"

#include <glib/gprintf.h>
#include <utility>
#include <vector>
#include <memory>
#include <string.h>

vector<shared_ptr<EditArea>>SectionData::AllEditArea = {};

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

map<string,GtkTextTagTable*> TagTables::LangTextTagTable = {};

void TagTables::AddToLang(const string &Lang, GtkTextTag* Tag){
    auto result = LangTextTagTable.find(Lang);
    if(result == LangTextTagTable.end()){
        //new language
        LangTextTagTable.emplace(Lang ,gtk_text_tag_table_new());
    }else{
        gtk_text_tag_table_add(result->second, Tag);
    }
}

GtkTextTagTable* TagTables::GetLangTagTable(const string &Lang){
    auto result = LangTextTagTable.find(Lang);
    if(result == LangTextTagTable.end()){
        //unknown language
        return NULL;
    }else{
        return result->second;
    }
}
