#ifndef EDITAREA_H_
#define EDITAREA_H_

#include <gtk/gtk.h>
#include <string>
#include <memory>

#include "../components/TextArea.h"


// Forward declaration
class SearchReplaceDialog;

class EditArea:public TextArea{
public:
    EditArea(GFile* file);
    ~EditArea();

    bool m_isSaved = true;

    void CountError();
    void LoadCursorPos();
    void LoadFile(GFile *file);

    GtkTextBuffer* getTextBuffer();
    GdkRectangle* getCursorRectangle();
    const char* getFilePath();

    void setLanguage(const char* lang) override;

    void Destroy() override;
    void Save();
    void ShowSearchDialog();
    void ShowReplaceDialog();

    //callbacks
    void UnfocusedCallback();
    void CursorMovedByKeyCallback();
    void TextChangedCallback();
    bool KeyInputCallback(guint keyval, guint keycode, GdkModifierType state);
    void CursorPosChangedCallback();
    void SaveButtonClickedCallback();
    void LangButtonClickedCallback();
    void FileSavedCallback(GFile *file);

private:
    GdkRectangle m_cursorRec;

    bool m_isCurMovedByKey = false;
    bool m_isTextChanged = false;
    bool m_pauseTextChangedCallback = false;

    std::string m_fileName;
    std::string m_absoPath;

    GFile *m_editingFile = nullptr;
    GtkGrid *m_baseGrid;
    GtkButton *m_locationBut;// showing path
    GtkButton *m_saveBut;
    GtkButton *m_errorBut;
    GtkLabel *m_errorButLabel;
    GtkButton *m_outlineBut;// not working now
    GtkButton *m_langBut;
    GtkButton *m_cursorPosBut;// showing cursor's current line and offset

    GtkEventController *m_keyDownEventCtrl;
    GtkEventController *m_focusEventCtrl;

    // Search and replace
    std::unique_ptr<SearchReplaceDialog> searchDialog;

    void LoadGui();
    void ConnectSignals();
};


#endif
