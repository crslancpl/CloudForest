#ifndef CLASSES_H_
#define CLASSES_H_
#pragma once

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <vector>
#include <map>
#include <utility>
#include <memory>
#include <string>

using namespace std;

class FPFolderButton;
class FPFileButton;
class FilePanel;

class EditAreaHolder;
class EditAreaHolderTabBut;

class EditArea;
class EditAreaHolderTabBut;



class Suggestion{
public:
    Suggestion(string *content, string *type, string *doc);
    string *Content;
    string *Type;
    string *Doc;
};






class MainWindow{
public:
    EditAreaHolder *EAHolder;
    GtkApplication *App;
    GtkWindow *Window;
    FilePanel *FP;
    GtkButton *AppBut;
    GtkButton *FileBut;
    GtkButton *CompileBut;
    GtkSearchEntry *SearchBar;
    GtkGrid *WindowGrid;
};

class HeaderBar{
public:
    GtkApplication *App;
    GtkHeaderBar *HeaderBar;
    GtkMenuButton *FileBut;
};


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
