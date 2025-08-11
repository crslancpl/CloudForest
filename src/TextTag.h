#ifndef TEXTTAG_H_
#define TEXTTAG_H_

#include "Classes.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

class TagStyle{
    public:
    TagStyle(const string &Name);
    string TagName;
    map<string,string> Styles;
    // Property, Value
    bool AddStyle(pair<string, string> Style);
};

class TagTables{
public:
    string Lang;
    vector<TagStyle> LangStyles;
    static map<string,TagTables*> LangTextTagsStyle;
    static void AddToLang(const string &Lang, TagStyle *Tag);
    static TagTables *GetLangTagTable(const string &Lang);
};

#endif
