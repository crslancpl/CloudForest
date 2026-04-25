#ifndef TEXTAREA_H_
#define TEXTAREA_H_

#include "CfContent.h"
#include "datatypes/language.h"

#include <gtk/gtk.h>
#include <unordered_set>

class TextArea : public CfContent{
public:
    TextArea();
    ~TextArea();

    char* GetContent();
    void SetContent(char *content);
    void SetEditable(bool editable);
    void SetFirstLineNumber(int number);
    datatypes::Language* GetLanguage();
    virtual void SetLanguage(datatypes::Language *lang);



    void AddLangChangedCallback(void (*callback)(TextArea*, datatypes::Language*));
    void RemoveLangChangedCallback(void (*callback)(TextArea*, datatypes::Language*));

    void ClearHighlight();
    void ApplyTagByLength(unsigned int textstartpos, unsigned int textlength, const char *tagname);
    void ApplyTagByPos(unsigned int textstartpos, unsigned int textendpos, const char *tagname);
    void ApplyTagByLinePos(unsigned int line, unsigned int pos, unsigned int length,const char *tagname);

    void CountLines();

protected:
    std::unordered_set<void (*)(TextArea*, datatypes::Language*)> m_langChangedCallbacks;

    datatypes::Language* m_language = nullptr;
    int m_firstLineNumber = 1;
    unsigned int m_totalLines = 0;
    unsigned int m_totalChars = 0;

    GtkTextView *m_textView;
    GtkTextBuffer *m_textViewBuffer;// contains file content
    GtkTextView *m_lineNoArea;
    GtkTextBuffer *m_lineNoAreaBuffer;
    GtkBox *m_baseBox;

    /* These iter are shared by multiple functions. ex: draw, delete line */
    GtkTextIter m_startItr, m_endItr;
    GtkTextIter m_cursorItr;

    unsigned int m_cursorPos = 0;
    unsigned int m_cursorLine = 0;
    unsigned int m_cursorColumn = 0;
};

namespace text_editor {
    void NewEmptyEditor();
    void NewEditor(GFile *file);
}

#endif
