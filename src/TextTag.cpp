#include "Classes.h"

map<string,TagTables*> TagTables::LangTextTagsStyle = {};

TagStyle::TagStyle(const string &Name){
    TagName = Name;
}

bool TagStyle::AddStyle(pair<string, string> Style){
    Styles.insert(Style);
    return true;
}

void TagTables::AddToLang(const string &Lang, TagStyle *Tag){
    g_print("%s", Tag->TagName.c_str());
    auto result = LangTextTagsStyle.find(Lang);
    if(result == LangTextTagsStyle.end()){
        //new language
        pair<string, TagTables*> NewLang;
        NewLang.first = Lang;
        g_print("%s", Tag->TagName.c_str());
        TagTables *tb = new TagTables;
        NewLang.second = tb;
        LangTextTagsStyle.insert(NewLang);

        NewLang.second->LangStyles.push_back(*Tag);
    }else{
        result->second->LangStyles.push_back(*Tag);
    }
}

TagTables* TagTables::GetLangTagTable(const string &Lang){
    auto result = LangTextTagsStyle.find(Lang);
    if(result == LangTextTagsStyle.end()){
        //unknown language
        g_print("NULL Tag table\n");
        return NULL;
    }else{
        g_print("found Tag table %s. Size: %d \n", result->first.c_str(),(int)result->second->LangStyles.size());
        return result->second;
    }
}
