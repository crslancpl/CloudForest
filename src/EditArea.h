#ifndef EDITAREA_H_
#define EDITAREA_H_
#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>

#include "Classes.h"

using namespace std;

/* EdiaArea class is defined in DataTypes.h */
class EditArea{
public:
    EditArea(GFile *file, FPFileButton* filebut);
    ~EditArea();
    char* RandomId;
    FPFileButton* CorreFileButton = nullptr;//Correspoding File Button
    EditAreaHolderTabBut* ParentSwitcher = nullptr;
    EditAreaHolder* ParentHolder = nullptr;
    bool IsSaved = true;

    GFile* EditingFile = nullptr;
    string Lang;

    GtkBuilder *builder;

    int cacheTotalLine = 0;
    char *FileName;
    char *RelePath;
    char *AbsoPath;
    int CursorPos;

    bool IsCurMovedByKey;

    GtkGrid *BaseGrid;
    GtkTextIter *Cursoritr;
    GtkButton *LocationBut;
    GtkButton *SaveBut;
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
    void Destroy();
    void Save();
    void LoadFile(GFile* newfile);
};

class EditAreaHolderTabBut{
    public:
    shared_ptr<EditArea> EA;
    EditAreaHolder* ParentHolder;
    GtkBox *BaseBox;
    GtkButton *Button;
    GtkButton *CloseButton;
    void Init(const shared_ptr<EditArea>& editarea, EditAreaHolder& parentholder);
};

class EditAreaHolder{
public:
    vector<shared_ptr<EditAreaHolderTabBut>> TabButtons;
    vector<shared_ptr<EditArea>> EditAreas;
    void Init();
    GtkStack *Container;
    GtkBox *Switcher;
    GtkGrid *BaseGrid;
    void Show(const shared_ptr<EditArea>& editarea);
    void NewTabButton();
};

static void RemoveTagFromTable(GtkTextTag* tag, GtkTextTagTable* table);
static void AddTagToTable(GtkTextTag* tag,GtkTextTagTable* table);
static void ChooseLang(GtkButton *self, EditArea* Parent);
static void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent);
static void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent);
static void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent);
static void SaveButClicked(GtkButton *self, EditArea* parent);
static void SwitcherButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent);
static void SwitcherCloseButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent);

#endif
