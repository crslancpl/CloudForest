#ifndef SEARCHREPLACEDIALOG_H_
#define SEARCHREPLACEDIALOG_H_

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <memory>
#include "../Classes.h"

class SearchReplaceDialog {
public:
    SearchReplaceDialog(EditArea* editArea);
    ~SearchReplaceDialog();

    void Show();
    void Hide();
    bool IsVisible() const;

private:
    EditArea* m_editArea;
    GtkWindow* m_dialog;
    GtkEntry* m_searchEntry;
    GtkEntry* m_replaceEntry;
    GtkCheckButton* m_caseSensitiveCheck;
    GtkCheckButton* m_regexCheck;
    GtkButton* m_findButton;
    GtkButton* m_replaceButton;
    GtkButton* m_replaceAllButton;
    GtkButton* m_closeButton;
    GtkLabel* m_statusLabel;

    // Search state
    GtkTextIter m_searchStart;
    GtkTextIter m_searchEnd;
    bool m_searchActive;
    std::string m_lastSearchText;
    bool m_lastCaseSensitive;
    bool m_lastRegex;

    // UI creation
    void CreateDialog();
    void CreateWidgets();
    void ConnectSignals();
    void SetupLayout();

    // Search functionality
    bool FindNext();
    bool FindPrevious();
    bool FindText(const std::string& text, bool caseSensitive, bool regex, bool forward = true);
    void HighlightMatch(const GtkTextIter& start, const GtkTextIter& end);
    void ClearHighlight();
    void UpdateStatus(const std::string& message);

    // Replace functionality
    void ReplaceCurrent();
    void ReplaceAll();
    bool ReplaceText(const GtkTextIter& start, const GtkTextIter& end, const std::string& replacement);

    // Static callbacks
    static void OnFindClicked(GtkButton* button, SearchReplaceDialog* dialog);
    static void OnReplaceClicked(GtkButton* button, SearchReplaceDialog* dialog);
    static void OnReplaceAllClicked(GtkButton* button, SearchReplaceDialog* dialog);
    static void OnCloseClicked(GtkButton* button, SearchReplaceDialog* dialog);
    static void OnSearchEntryActivated(GtkEntry* entry, SearchReplaceDialog* dialog);
    static void OnReplaceEntryActivated(GtkEntry* entry, SearchReplaceDialog* dialog);
    static void OnKeyPressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state, SearchReplaceDialog* dialog);
    static void OnDialogResponse(GtkDialog* dialog, int response_id, SearchReplaceDialog* self);
};

#endif
