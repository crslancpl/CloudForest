#include "TextArea.h"

#include "datatypes/common.h"
#include "datatypes/language.h"
#include "src/gui/style/Style.h"

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <string>

/*
 * Static
 */

static void TextChanged(GtkTextBuffer *textviewbuffer, TextArea *textarea){
    textarea->CountLines();
}

/*
 * Public
 */

TextArea::TextArea(){
    GtkBuilder *builder = gtk_builder_new_from_file("data/ui/TextArea.ui");

    m_baseBox = GTK_BOX(gtk_builder_get_object(builder, "BaseBox"));
    m_textView = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "TextArea"));
    m_textViewBuffer = gtk_text_view_get_buffer(m_textView);
    m_lineNoArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "LineNum"));
    m_lineNoAreaBuffer = gtk_text_view_get_buffer(m_lineNoArea);

    gtk_scrollable_set_vadjustment(GTK_SCROLLABLE(m_lineNoArea), gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(m_textView)));
    gtk_text_view_set_bottom_margin (m_textView, 80);
    gtk_text_view_set_bottom_margin (m_lineNoArea, 80);
    gtk_text_view_set_pixels_below_lines(m_textView, 5);
    gtk_text_view_set_pixels_below_lines(m_lineNoArea, 5);
    gtk_text_view_set_accepts_tab(m_textView, false);

    style::LoadTextTag(m_textViewBuffer);
    g_signal_connect(m_textViewBuffer, "changed", G_CALLBACK(TextChanged), this);

    g_object_ref(m_baseBox);
    SetContentWidget(GTK_WIDGET(m_baseBox));
    CountLines();

    g_object_unref(builder);
}

TextArea::~TextArea(){
    //
}

void TextArea::SetContent(char *content){
    gtk_text_buffer_set_text(m_textViewBuffer, content, -1);
}

char* TextArea::GetContent(){
    gtk_text_buffer_get_bounds(m_textViewBuffer, &m_startItr, &m_endItr);
    return gtk_text_buffer_get_text(m_textViewBuffer, &m_startItr, &m_endItr, true);
}

void TextArea::SetEditable(bool editable){
    gtk_text_view_set_editable(m_textView, editable);
}

void TextArea::SetFirstLineNumber(int number){
    m_firstLineNumber = number;
    CountLines();
}

Language *TextArea::GetLanguage(){
    return m_language;
}

void TextArea::SetLanguage(Language *lang){
    m_language = lang;
    //call callbacks
    for (auto callback : m_langChangedCallbacks) {
        callback(this, lang);
    }
}

void TextArea::ClearHighlight(){
    gtk_text_buffer_get_bounds(m_textViewBuffer, &m_startItr, &m_endItr);
    gtk_text_buffer_remove_all_tags(m_textViewBuffer, &m_startItr, &m_endItr);
}

void TextArea::ApplyTagByLength(unsigned int textstartpos, unsigned int textlength, const char *tagname){
    /*
     * Position 1 is the position of the first character in the buffer
     */
    gtk_text_buffer_get_iter_at_offset(m_textViewBuffer, &m_startItr, textstartpos);
    gtk_text_buffer_get_iter_at_offset(m_textViewBuffer, &m_endItr, textstartpos + textlength);
    gtk_text_buffer_apply_tag_by_name(m_textViewBuffer, tagname, &m_startItr, &m_endItr);
}

void TextArea::ApplyTagByPos(unsigned int textstartpos, unsigned int textendpos, const char *tagname){
    /*
     * Position 1 is the position of the first character in the buffer
     */
    textstartpos --;
    gtk_text_buffer_get_iter_at_offset(m_textViewBuffer, &m_startItr, textstartpos);
    gtk_text_buffer_get_iter_at_offset(m_textViewBuffer, &m_endItr, textendpos);
    gtk_text_buffer_apply_tag_by_name(m_textViewBuffer, tagname, &m_startItr, &m_endItr);
}

void TextArea::ApplyTagByLinePos(unsigned int line, unsigned int pos, unsigned int length, const char *tagname){
    /*
     * Line 1 is the first line, and position 1 is the position
     * of the first character in the line So line and position shouldn't
     * be less then 1.
     */
    line--;// change the lines that starts from 1 to 0

    if (pos <= 0 || strcmp(tagname, "invisible")== 0) {
        return;
    }

    gtk_text_buffer_get_iter_at_line_offset(m_textViewBuffer, &m_startItr, line, pos -1 );
    gtk_text_iter_assign(&m_endItr, &m_startItr);
    gtk_text_iter_forward_chars(&m_endItr, length);
    gtk_text_buffer_apply_tag_by_name(m_textViewBuffer, tagname, &m_startItr, &m_endItr);
}

void TextArea::CountLines(){
    int NewLineCount = gtk_text_buffer_get_line_count(m_textViewBuffer);
    m_totalChars = gtk_text_buffer_get_char_count(m_textViewBuffer);

    if(NewLineCount != m_totalLines){

        gtk_text_buffer_get_end_iter(m_lineNoAreaBuffer, &m_startItr);
        if(NewLineCount > m_totalLines){
            while(m_totalLines < NewLineCount){
                std::string Line = std::to_string(m_totalLines + m_firstLineNumber) + '\n';
                gtk_text_buffer_insert(m_lineNoAreaBuffer, &m_startItr, Line.c_str(), -1);
                m_totalLines ++;
            }
        }else{
            gtk_text_buffer_get_iter_at_line(m_lineNoAreaBuffer, &m_endItr, NewLineCount);
            gtk_text_buffer_delete(m_lineNoAreaBuffer, &m_endItr, &m_startItr);
            m_totalLines = NewLineCount;
        }
    }
}



void TextArea::ListenEvent(Event event, EventCallback callback){
    switch (event) {
    case TEXTAREA_CLASS_LANG_CHANGED:
        m_langChangedCallbacks.insert((LangChangedCallback)callback);
        break;
    default:
        break;
    }
}

void TextArea::StopListenEvent(Event event, EventCallback callback){
    switch (event) {
    case TEXTAREA_CLASS_LANG_CHANGED:
        m_langChangedCallbacks.erase((LangChangedCallback)callback);
        break;
    default:
        break;
    }
}
