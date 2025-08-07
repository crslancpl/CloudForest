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

class Suggestion{
public:
    Suggestion(string *content, string *type, string *doc);
    string *Content;
    string *Type;
    string *Doc;
};

class EditArea{
public:
    EditArea(GFile *File);
    ~EditArea();

    string Lang;

    GtkBuilder *builder;

    int cacheTotalLine;
    char *FileName;
    char *RelePath;
    char *AbsoPath;
    int CursorPos;
    GtkTextIter *Cursoritr;
    bool IsCurMovedByKey;

    GtkGrid *BaseGrid;

    GtkButton *LocationBut;
    GtkButton *ErrorBut;
    GtkLabel *ErrorButLabel;
    GtkButton *OutlineBut;
    GtkMenuButton *LangBut;
    GMenu *LangMenu;
    GtkButton *CursorPosBut;

    GtkTextView *TextView;
    GtkTextBuffer *TextViewBuffer;
    GtkTextView *LineNoArea;
    GtkTextBuffer *LineNoAreaBuffer;

    GtkWindow *win;

    /* These iter are shared by multiple functions. ex: draw, delete line */
    GtkTextIter *StartItr;
    GtkTextIter *EndItr;

    void UnrefBuilder();
    void CountLine();
    void CountError();
    void LoadCursorPos();
    void ShowTip(char *Text);
    void ShowSuggestion(const vector<shared_ptr<Suggestion>> &Suggestions);// auto complete
    void ChangeLanguage();
    void ApplyTagByLength(int TextStartPos, int TextLength, char *TagName);
    void ApplyTagByPos(int TextStartPos, int TextEndPos, char *TagName);
};

class EditAreaHolder;

class EditAreaHolderTabBut{
    public:
    shared_ptr<EditArea> EA;
    EditAreaHolder* ParentHolder;
    GtkButton *Button;
    void Init(const shared_ptr<EditArea>& editarea, EditAreaHolder& parentholder);
};

class EditAreaHolder{
public:
    vector<shared_ptr<EditAreaHolderTabBut>> TabButtons;
    void Init();
    GtkStack *Container;
    GtkBox *Switcher;
    GtkGrid *BaseGrid;
    void SwitchTo(const string &RelePath);
    void Show(const shared_ptr<EditArea>& editarea);
    void NewTabButton();
};

class File {
public:
    GFile *file;
    shared_ptr<EditArea> ea;
    GtkButton *FileButton;
    char *FileAbsoPath;
    char *FileRelePath;
    char *FileName;

    void init(GFile *FileGFile,int level);
    void Open();
};

class Folder {
public:
    bool ChildLoaded = false;
    GtkBuilder *builder;
    bool Inited = false;
    char *FolderName;
    GtkBox *BaseBox;
    GtkButton *FolderToggleBut;
    GtkBox *Content;
    GFile *f;
    void init(GFile &Folder,GFile *Parent,int level);
    void AddChildFolder(Folder& Child);
    void AddChildFile(File& Child);
    void SetAsRoot(GtkBox *Box);
    int Level;

    bool IsOpen=false;
};


class FilePanel{
public:
    static int OffSet;
    void init();
    GtkGrid *BaseGrid;
    GtkBox *FileTree;
    Folder& NewFolder(GFile *file,GFile *ParentFolder,Folder& Parent);// Not nullable. The root folder will not be added with this function
    File& NewFile(GFile *file, Folder& Parent);// Not nullable. File has to be under a folder
    void SetParent(GFile *file);
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

class SectionData{
public:
    static vector<shared_ptr<EditArea>> AllEditArea;
    static vector<shared_ptr<Folder>> AllFolder;
    static vector<shared_ptr<File>> AllFile;
    static int EditAreaNum;
    static void RemoveEditArea(shared_ptr<EditArea> EditAreaPtr);
    static void AddEditArea(shared_ptr<EditArea> EditAreaPtr);
    static shared_ptr<EditArea> GetEditAreaFromFileAbsoPath(const string &AbsPath);
    static shared_ptr<File> AddFile();// Not nullable
    static shared_ptr<Folder> AddFolder();// Not nullable
    static void RemoveFolder(Folder& folder);// Not nullable
    static MainWindow currentwindow;
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
