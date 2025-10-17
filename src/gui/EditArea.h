#ifndef EDITAREA_H_
#define EDITAREA_H_

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <memory>

#include "../Classes.h"
#include "../Types.h"

// Forward declaration
class SearchReplaceDialog;

class EditArea{
public:
    EditArea(GFile *file);
    ~EditArea();

    EditAreaHolderTabBut* m_ParentSwitcher = nullptr;
    EditAreaHolder* m_ParentHolder = nullptr;
    TipPopover* m_Tippopover;
    SuggestionPopover* m_Sugpopover;

    GdkRectangle *m_CursorRec;

    bool m_IsSaved = true;
    bool m_IsCurMovedByKey = false;
    bool m_IsTextChanged = false;
    bool m_PauseTextChangedCallback = false;
    unsigned int m_cacheTotalLine = 0;
    unsigned int m_CursorPos;
    unsigned int m_CursorLine;
    unsigned int m_CursorLinePos;


    std::string m_Language;
    std::string m_FileName;
    //std::string RelaPath;
    std::string m_AbsoPath;

    GtkBuilder *m_Builder;
    GFile *m_EditingFile = nullptr;
    GtkGrid *m_BaseGrid;
    GtkButton *m_LocationBut;// showing path
    GtkButton *m_SaveBut;
    GtkButton *m_ErrorBut;
    GtkLabel *m_ErrorButLabel;
    GtkButton *m_OutlineBut;// not working now
    GtkButton *m_LangBut;
    GtkButton *m_CursorPosBut;// showing cursor's current line and offset
    GtkTextView *m_TextView;
    GtkTextBuffer *m_TextViewBuffer;// contains file content
    GtkTextView *m_LineNoArea;
    GtkTextBuffer *m_LineNoAreaBuffer;
    GtkEventController *m_KeyDownEventCtrl;
    GtkEventController *m_FocusEventCtrl;

    /* These iter are shared by multiple functions. ex: draw, delete line */
    GtkTextIter *CursorItr,*StartItr,*EndItr;

    // Search and replace
    std::unique_ptr<SearchReplaceDialog> searchDialog;

    void UnrefBuilder();
    void CountLine();
    void CountError();
    void LoadCursorPos();
    const std::string& GetContent();

    void ClearSuggestion();
    void AddSuggestion(Suggestion* sug);
    void ShowSuggestion();// auto complete
    void HideSuggestion();

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

private:
    void LoadGui();
    void ConnectSignals();
};

class EditAreaHolderTabBut{
public:
    std::shared_ptr<EditArea> m_CorrespondingEA;
    EditAreaHolder* m_ParentHolder;
    GtkBox *m_BaseBox;
    GtkButton *m_Button;
    GtkButton *m_CloseButton;
    void Init(const std::shared_ptr<EditArea>& editarea, EditAreaHolder& parentholder);
};

class EditAreaHolder{
public:
    GtkStack *m_Container;
    GtkBox *m_Switcher;
    GtkGrid *m_BaseGrid;

    void Init();
    void Show(const std::shared_ptr<EditArea>& editarea);
    void Remove(EditArea* editarea);
    std::shared_ptr<EditArea> GetCurrentEditArea();
private:
    std::vector<std::shared_ptr<EditAreaHolderTabBut>> m_TabButtons;
    //std::vector<std::shared_ptr<EditArea>> EditAreas;
    void NewTabButton();
};


#endif
