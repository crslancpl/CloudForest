#ifndef DATATYPES_H_
#define DATATYPES_H_
#pragma once

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <vector>
#include <map>
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
    void ShowTip(char *Text);
    void ShowSuggestion(const vector<shared_ptr<Suggestion>> &Suggestions);// auto complete
    void DrawColorByLength(int TextStartPos, int TextLength, char *TagName);
    void DrawColorByPos(int TextStartPos, int TextEndPos, char *TagName);
};

class EditAreaHolder{
public:
    void Init();
    GtkStack *Container;
    GtkBox *Switcher;
    GtkGrid *BaseGrid;
    void SwitchTo(const string &RelePath);
    void Show(shared_ptr<EditArea> editarea);
};
class Folder {
public:
    GtkBuilder *builder;
    bool Inited = false;
    char *FolderName;
    GtkBox *BaseBox;
    GtkButton *FolderToggleBut;
    GtkBox *Content;
    void init(GFile *Folder,GFile *Parent,int level);
    void AddChildFolder(shared_ptr<Folder> Child);
    void SetAsRoot(GtkBox *Box);
    int Level;
    static int OffSet;
    bool IsOpen=true;
};

class File {
public:
    GFile *file;
    shared_ptr<EditArea> ea;
    GtkButton *FileButton;
    char *FileAbsoPath;
    char *FileRelePath;
    char *FileName;

    void init(GFile *FileGFile);
    void Open();
};

class FilePanel{
public:

    void init();
    GtkGrid *BaseGrid;
    GtkBox *FileTree;
    shared_ptr<Folder> NewFolder(GFile *File,GFile *ParentFolder,shared_ptr<Folder> Parent);
    void NewFile(GFile *File, shared_ptr<Folder> Parent);
    void SetParent(GFile *File);
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
    static int EditAreaNum;
    static void RemoveEditArea(shared_ptr<EditArea> EditAreaPtr);
    static void AddEditArea(shared_ptr<EditArea> EditAreaPtr);
    static shared_ptr<EditArea> GetEditAreaFromFileAbsoPath(const string &AbsPath);

    static void RemoveFolder(shared_ptr<Folder> Folder);
    static void AddFolder(shared_ptr<Folder> Folder);
};

class TagTables{
public:
    static map<string,GtkTextTagTable*> LangTextTagTable;
    static void AddToLang(const string &Lang, GtkTextTag* Tag);
    static GtkTextTagTable *GetLangTagTable(const string &Lang);
};

#endif
