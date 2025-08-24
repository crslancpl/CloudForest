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
        return nullptr;
    }else{
        return result->second;
    }
}

void LoadDefaultTag(GtkTextBuffer*buffer){
    gtk_text_buffer_create_tag(buffer, "type", "foreground","lime", nullptr);
    gtk_text_buffer_create_tag(buffer, "keyword", "foreground","cyan", nullptr);
    gtk_text_buffer_create_tag(buffer, "none", "foreground","white", nullptr);
    gtk_text_buffer_create_tag(buffer, "cmt", "foreground","rgb(58,193,114)", nullptr);
    gtk_text_buffer_create_tag(buffer, "scmt", "foreground","orange", nullptr);
    gtk_text_buffer_create_tag(buffer, "text", "foreground","magenta", nullptr);
    gtk_text_buffer_create_tag(buffer, "tag", "foreground","cyan", nullptr);
    gtk_text_buffer_create_tag(buffer, "func", "foreground","rgb(199,176,252)", nullptr);
    gtk_text_buffer_create_tag(buffer, "num", "foreground","rgb(128,188,237)", nullptr);
}
