#ifndef EDITAREA_H_
#define EDITAREA_H_

#include "../components/TextArea.h"

#include <gtk/gtk.h>
#include <memory>
#include <unordered_set>

// Forward declaration
class DiagnosticPopover;
class SearchReplaceDialog;
typedef struct Diagnostic Diagnostic;
typedef struct FileData FileData;

class EditArea:public TextArea{
public:
    EditArea(FileData* file);
    ~EditArea();

    bool isSaved = true;

    //override
    void SetLanguage(Language*) override;
    void Destroy() override;

    GtkTextBuffer* GetTextBuffer();
    GdkRectangle* GetCursorRectangle();
    const char* GetFilePath() const;
    const unsigned int GetFileVersion() const;

    void AddDiagnostic(Diagnostic* diagnostic); // add a diagnostic
    void ProcessDiagnostics(); // read data from all diagnostics and show highlight
    void ClearDiagnostics(); // clear the diagnostics list
    Diagnostic* FindDiagnostic(GtkTextIter* itr);
    void LoadCursorPos();
    void LoadFile(FileData *file);
    void Save();
    void ShowSearchDialog();
    void ShowReplaceDialog();
    void Insert(unsigned int line, unsigned int column, const char* text);
    void InsertAtCursor(const char* text);

    //callbacks
    bool KeyInput(guint keyval, guint keycode, GdkModifierType state);
    void Unfocused();
    void MouseMoved(double x, double y);
    void CursorMovedByKey();
    void TextChanged();
    void LangChanged();
    void CursorPosChanged();
    void SaveButtonClicked();
    void LangButtonClicked();
    void FileSaved(FileData *file);

private:
    GdkRectangle m_cursorRec;

    DiagnosticPopover* m_diagnosticPopover;
    Range m_currentDiagnosticRange;
    bool m_isCurMovedByKey = false;
    bool m_isTextChanged = false;

    char *m_originalContent = nullptr;
    unsigned int m_originalLength = 0;
    unsigned int m_fileVersion = 0;

    FileData *m_editingFile = nullptr;
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
    GtkEventController *m_mouseMovedEventCtrl;

    std::unordered_set<Diagnostic*> m_diagnosticsList;

    // Search and replace
    std::unique_ptr<SearchReplaceDialog> searchDialog;

    void LoadGui();
    void ConnectSignals();
};


#endif
