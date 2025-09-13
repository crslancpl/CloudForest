#ifndef EDITAREA_H_
#define EDITAREA_H_

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>

#include "../Classes.h"

// Forward declaration
class SearchReplaceDialog;

/* EdiaArea class is defined in DataTypes.h */
class EditArea{
public:
    std::vector<std::string> TextChangedPyCallback;
    EditArea(GFile *file);
    ~EditArea();

    EditAreaHolderTabBut* ParentSwitcher = nullptr;
    EditAreaHolder* ParentHolder = nullptr;

    bool IsSaved = true;
    bool IsCurMovedByKey;
    int cacheTotalLine = 0;
    int CursorPos;


    std::string RandomId;
    std::string Language;
    std::string FileName;
    std::string RelaPath;
    std::string AbsoPath;

    GtkBuilder *builder;
    GFile* EditingFile = nullptr;
    GtkGrid *BaseGrid;
    GtkTextIter *Cursoritr;
    GtkButton *LocationBut;// showing path
    GtkButton *SaveBut;
    GtkButton *ErrorBut;
    GtkLabel *ErrorButLabel;
    GtkButton *OutlineBut;// not working now
    GtkButton *LangBut;
    GtkButton *CursorPosBut;// showing cursor's current line and offset
    GtkTextView *TextView;
    GtkTextBuffer *TextViewBuffer;// contains file content
    GtkTextView *LineNoArea;
    GtkTextBuffer *LineNoAreaBuffer;
    GtkEventController* KeyDownEventCtrl;

    /* These iter are shared by multiple functions. ex: draw, delete line */
    GtkTextIter *StartItr;
    GtkTextIter *EndItr;

    // Search and replace
    std::unique_ptr<SearchReplaceDialog> searchDialog;

    void UnrefBuilder();
    void CountLine();
    void CountError();
    void LoadCursorPos();
    const std::string& GetContent();
    void AddTextChangedPyCalback(std::string funcname);
    void ShowTip(char *Text);// not working
    void ShowSuggestion(const std::vector<std::shared_ptr<Suggestion>> &Suggestions);// auto complete
    void ChangeLanguage(const std::string& lang, bool highlight);//pass false to highlight if you don't want to highlight now
    void HighlightSyntax();
    void ApplyTagByLength(int TextStartPos, int TextLength, char *TagName);
    void ApplyTagByPos(int TextStartPos, int TextEndPos, char *TagName);
    void ApplyTagByLinePos(int line, int offset, int length,char *TagName);
    void Destroy();
    void Save();
    void LoadFile(GFile* newfile);
    void ShowSearchDialog();
    void ShowReplaceDialog();
};

class EditAreaHolderTabBut{
public:
    std::shared_ptr<EditArea> EA;
    EditAreaHolder* ParentHolder;
    GtkBox *BaseBox;
    GtkButton *Button;
    GtkButton *CloseButton;
    void Init(const std::shared_ptr<EditArea>& editarea, EditAreaHolder& parentholder);
};

class EditAreaHolder{
public:
    GtkStack *Container;
    GtkBox *Switcher;
    GtkGrid *BaseGrid;

    void Init();
    void Show(const std::shared_ptr<EditArea>& editarea);
    void Remove(EditArea* editarea);
    std::shared_ptr<EditArea> GetCurrentEditArea();
private:
    std::vector<std::shared_ptr<EditAreaHolderTabBut>> TabButtons;
    //std::vector<std::shared_ptr<EditArea>> EditAreas;
    void NewTabButton();
};


#endif
