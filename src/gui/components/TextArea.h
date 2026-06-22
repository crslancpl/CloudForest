#ifndef TEXTAREA_H_
#define TEXTAREA_H_

#include "CfContent.h"
#include "datatypes/common.h"
#include "toolset/event/Event.h"

#include <gtk/gtk.h>


//forward declaration
class TextArea;
typedef struct ZRange ZRange;
typedef struct Language Language;
typedef void(*LangChangedCallback)(TextArea*, Language*);



class TextArea : public CfContent{
public:
    TextArea();
    ~TextArea();

    const char* GetContent();
    void SetContent(char *content);
    void SetEditable(bool editable);
    void SetFirstLineNumber(int number);
    const Language* GetLanguage() const;
    virtual void SetLanguage(Language *lang);

    void ClearHighlight();
    void ApplyTagByRange(ZRange *range, const char *tagname);
    void ApplyTagByLength(unsigned int textstartpos, unsigned int textlength, const char *tagname);
    void ApplyTagByPos(unsigned int textstartpos, unsigned int textendpos, const char *tagname);
    void ApplyTagByLinePos(unsigned int line, unsigned int pos, unsigned int length,const char *tagname);
    void CountLines();

    enum TextAreaSignal{
        TEXTAREA_CLASS_LANG_CHANGED
    };

    void Listen(TextAreaSignal signal, EventCallback callback);
    void StopListen(TextAreaSignal signal, EventCallback callback);

protected:
    std::unordered_map<TextAreaSignal, SimpleEvent> m_eventMap;

    Language* m_language = nullptr;
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

    unsigned int m_cursorIndex = 0;
    ZPosition m_cursorZPos;
};

#endif
