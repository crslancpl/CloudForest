#include "TextTag.h"


map<string,TagTables*> TagTables::LangTextTagsStyle = {};

TagStyle::TagStyle(const string &Name){
    TagName = Name;
}

bool TagStyle::AddStyle(pair<string, string> Style){
    Styles.insert(Style);
    return true;
}

void TagTables::AddToLang(const string &Lang, TagStyle *Tag){
    auto result = LangTextTagsStyle.find(Lang);
    if(result == LangTextTagsStyle.end()){
        //new language
        pair<string, TagTables*> NewLang;
        NewLang.first = Lang;
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
        return NULL;
    }else{
        return result->second;
    }
}
