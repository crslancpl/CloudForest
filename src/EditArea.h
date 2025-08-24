#ifndef EDITAREA_H_
#define EDITAREA_H_
#include "cf/CFEmbed.h"
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
    string RandomId;
    FPFileButton* CorreFileButton = nullptr;//Correspoding File Button
    EditAreaHolderTabBut* ParentSwitcher = nullptr;
    EditAreaHolder* ParentHolder = nullptr;
    bool IsSaved = true;

    GFile* EditingFile = nullptr;
    string Language;

    GtkBuilder *builder;

    int cacheTotalLine = 0;
    char *FileName;
    char *RelePath;
    char *AbsoPath;
    int CursorPos;

    bool IsCurMovedByKey;

    GtkGrid *BaseGrid;
    GtkTextIter *Cursoritr;
    GtkButton *LocationBut;// showing path
    GtkButton *SaveBut;
    GtkButton *ErrorBut;
    GtkLabel *ErrorButLabel;
    GtkButton *OutlineBut;// not working now
    GtkButton *LangBut;// not working now
    GMenu *LangMenu;// not working now
    GtkButton *CursorPosBut;// showing cursor's current line and offset

    GtkTextView *TextView;
    GtkTextBuffer *TextViewBuffer;// contains file content
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
    void ShowTip(char *Text);// not working
    void ShowSuggestion(const vector<shared_ptr<Suggestion>> &Suggestions);// auto complete
    void ChangeLanguage(const string& lang, bool highlight);//pass false to highlight if you don't want to highlight now
    void HighlightSyntax();
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

void RemoveTagFromTable(GtkTextTag* tag, GtkTextTagTable* table);// bugged
void AddTagToTable(GtkTextTag* tag,GtkTextTagTable* table);
void ChooseLang(GtkButton *self, EditArea* Parent);// not used now
void CursorMovedByKey(GtkTextView* self, GtkMovementStep* step, gint count, gboolean extend_selection, EditArea *Parent);
void TextChanged(GtkTextBuffer* buffer, GParamSpec* pspec, EditArea* Parent);
void CursorPosChanged (GtkTextBuffer *buffer, GParamSpec *pspec G_GNUC_UNUSED, EditArea *Parent);
void SaveButtonClicked(GtkButton *self, EditArea* parent);
void SwitcherButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent);
void SwitcherCloseButtonClicked(GtkButton *self, EditAreaHolderTabBut* Parent);

#endif
