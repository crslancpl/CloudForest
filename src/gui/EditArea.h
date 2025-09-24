#ifndef EDITAREA_H_
#define EDITAREA_H_

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>

#include "../Classes.h"

#include "../lsp/LSP.h"

// Forward declaration
class SearchReplaceDialog;

/* EdiaArea class is defined in DataTypes.h */
class EditArea{
public:
    std::vector<std::string> TextChangedPyCallback;
    EditArea(GFile *file);
    ~EditArea();

    LSPServer* LSPserver;

    EditAreaHolderTabBut* ParentSwitcher = nullptr;
    EditAreaHolder* ParentHolder = nullptr;
    TipPopover* Tippopover;
    SuggestionPopover* Sugpopover;

    GdkRectangle *CursorRec;

    bool IsSaved = true;
    bool IsCurMovedByKey = false;
    bool IsTextChanged = false;
    unsigned int cacheTotalLine = 0;
    unsigned int CursorPos;
    unsigned int CursorLine;
    unsigned int CursorLinePos;


    std::string Language;
    std::string FileName;
    //std::string RelaPath;
    std::string AbsoPath;

    GtkBuilder *builder;
    GFile *EditingFile = nullptr;
    GtkGrid *BaseGrid;
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
    GtkEventController *KeyDownEventCtrl;
    GtkEventController *FocusEventCtrl;

    /* These iter are shared by multiple functions. ex: draw, delete line */
    GtkTextIter *CursorItr,*StartItr,*EndItr;

    // Search and replace
    std::unique_ptr<SearchReplaceDialog> searchDialog;

    void UnrefBuilder();
    void CountLine();
    void CountError();
    void LoadCursorPos();
    const std::string& GetContent();
    void AddTextChangedPyCalback(std::string funcname);
    void ShowTip(char *Text);// not working
    void ShowSuggestion(const std::vector<std::shared_ptr<Suggestion>> *Suggestions);// auto complete
    void ChangeLanguage(const std::string& lang, bool highlight);//pass false to highlight if you don't want to highlight now
    void HighlightSyntax();
    void ApplyTagByLength(unsigned int TextStartPos, unsigned int TextLength, char *TagName);
    void ApplyTagByPos(unsigned int TextStartPos, unsigned int TextEndPos, char *TagName);
    void ApplyTagByLinePos(unsigned int line, unsigned int pos, unsigned int length,char *TagName);
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
