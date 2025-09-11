#include "SearchReplaceDialog.h"
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <regex>
#include <algorithm>
#include "EditArea.h"

SearchReplaceDialog::SearchReplaceDialog(EditArea* editArea) 
    : m_editArea(editArea), m_dialog(nullptr), m_searchActive(false) {
    CreateDialog();
}

SearchReplaceDialog::~SearchReplaceDialog() {
    if (m_dialog) {
        gtk_window_destroy(GTK_WINDOW(m_dialog));
    }
}

void SearchReplaceDialog::CreateDialog() {
    m_dialog = GTK_DIALOG(gtk_dialog_new());
    gtk_window_set_title(GTK_WINDOW(m_dialog), "Find and Replace");
    gtk_window_set_modal(GTK_WINDOW(m_dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(m_dialog), GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(m_editArea->TextView), GTK_TYPE_WINDOW)));
    gtk_window_set_default_size(GTK_WINDOW(m_dialog), 500, 200);
    gtk_window_set_resizable(GTK_WINDOW(m_dialog), FALSE);

    CreateWidgets();
    SetupLayout();
    ConnectSignals();

    // Initialize search state
    gtk_text_buffer_get_selection_bounds(m_editArea->TextViewBuffer, &m_searchStart, &m_searchEnd);
    if (gtk_text_iter_equal(&m_searchStart, &m_searchEnd)) {
        gtk_text_buffer_get_iter_at_mark(m_editArea->TextViewBuffer, &m_searchStart, 
                                        gtk_text_buffer_get_insert(m_editArea->TextViewBuffer));
    }
}

void SearchReplaceDialog::CreateWidgets() {
    // Search entry
    m_searchEntry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(m_searchEntry, "Search for...");
    gtk_widget_set_hexpand(GTK_WIDGET(m_searchEntry), TRUE);

    // Replace entry
    m_replaceEntry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(m_replaceEntry, "Replace with...");
    gtk_widget_set_hexpand(GTK_WIDGET(m_replaceEntry), TRUE);

    // Check buttons
    m_caseSensitiveCheck = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Case sensitive"));
    m_regexCheck = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Regular expression"));

    // Buttons
    m_findButton = GTK_BUTTON(gtk_button_new_with_label("Find Next"));
    m_replaceButton = GTK_BUTTON(gtk_button_new_with_label("Replace"));
    m_replaceAllButton = GTK_BUTTON(gtk_button_new_with_label("Replace All"));
    m_closeButton = GTK_BUTTON(gtk_button_new_with_label("Close"));

    // Status label
    m_statusLabel = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_xalign(m_statusLabel, 0.0);
}

void SearchReplaceDialog::SetupLayout() {
    GtkWidget* contentArea = gtk_dialog_get_content_area(m_dialog);
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_margin_top(grid, 10);
    gtk_widget_set_margin_bottom(grid, 10);
    gtk_widget_set_margin_start(grid, 10);
    gtk_widget_set_margin_end(grid, 10);

    // Search row
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Find:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(m_searchEntry), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(m_findButton), 2, 0, 1, 1);

    // Replace row
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Replace:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(m_replaceEntry), 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(m_replaceButton), 2, 1, 1, 1);

    // Options row
    GtkWidget* optionsBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(optionsBox), GTK_WIDGET(m_caseSensitiveCheck));
    gtk_box_append(GTK_BOX(optionsBox), GTK_WIDGET(m_regexCheck));
    gtk_grid_attach(GTK_GRID(grid), optionsBox, 0, 2, 2, 1);

    // Replace All button
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(m_replaceAllButton), 2, 2, 1, 1);

    // Status row
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(m_statusLabel), 0, 3, 3, 1);

    gtk_box_append(GTK_BOX(contentArea), grid);

    // Add close button to action area
    gtk_dialog_add_button(m_dialog, "Close", GTK_RESPONSE_CLOSE);
}

void SearchReplaceDialog::ConnectSignals() {
    g_signal_connect(m_findButton, "clicked", G_CALLBACK(OnFindClicked), this);
    g_signal_connect(m_replaceButton, "clicked", G_CALLBACK(OnReplaceClicked), this);
    g_signal_connect(m_replaceAllButton, "clicked", G_CALLBACK(OnReplaceAllClicked), this);
    g_signal_connect(m_closeButton, "clicked", G_CALLBACK(OnCloseClicked), this);
    g_signal_connect(m_searchEntry, "activate", G_CALLBACK(OnSearchEntryActivated), this);
    g_signal_connect(m_replaceEntry, "activate", G_CALLBACK(OnReplaceEntryActivated), this);
    g_signal_connect(m_dialog, "response", G_CALLBACK(OnDialogResponse), this);

    // Keyboard shortcuts
    GtkEventController* keyController = gtk_event_controller_key_new();
    g_signal_connect(keyController, "key-pressed", G_CALLBACK(OnKeyPressed), this);
    gtk_widget_add_controller(GTK_WIDGET(m_dialog), keyController);
}

void SearchReplaceDialog::Show() {
    if (m_dialog) {
        gtk_widget_set_visible(GTK_WIDGET(m_dialog), TRUE);
        gtk_widget_grab_focus(GTK_WIDGET(m_searchEntry));
    }
}

void SearchReplaceDialog::Hide() {
    if (m_dialog) {
        ClearHighlight();
        gtk_widget_set_visible(GTK_WIDGET(m_dialog), FALSE);
    }
}

bool SearchReplaceDialog::IsVisible() const {
    return m_dialog && gtk_widget_get_visible(GTK_WIDGET(m_dialog));
}

bool SearchReplaceDialog::FindNext() {
    const char* searchText = gtk_editable_get_text(GTK_EDITABLE(m_searchEntry));
    if (!searchText || strlen(searchText) == 0) {
        UpdateStatus("Please enter search text");
        return false;
    }

    bool caseSensitive = gtk_check_button_get_active(m_caseSensitiveCheck);
    bool regex = gtk_check_button_get_active(m_regexCheck);

    return FindText(searchText, caseSensitive, regex, true);
}

bool SearchReplaceDialog::FindPrevious() {
    const char* searchText = gtk_editable_get_text(GTK_EDITABLE(m_searchEntry));
    if (!searchText || strlen(searchText) == 0) {
        UpdateStatus("Please enter search text");
        return false;
    }

    bool caseSensitive = gtk_check_button_get_active(m_caseSensitiveCheck);
    bool regex = gtk_check_button_get_active(m_regexCheck);

    return FindText(searchText, caseSensitive, regex, false);
}

bool SearchReplaceDialog::FindText(const std::string& text, bool caseSensitive, bool regex, bool forward) {
    GtkTextIter start, end;
    GtkTextIter searchStart = m_searchStart;
    GtkTextIter searchEnd = m_searchEnd;

    // Get the full text buffer
    gtk_text_buffer_get_bounds(m_editArea->TextViewBuffer, &start, &end);
    std::string bufferText = gtk_text_buffer_get_text(m_editArea->TextViewBuffer, &start, &end, TRUE);

    // Prepare search text
    std::string searchText = text;
    if (!caseSensitive) {
        std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);
        std::transform(bufferText.begin(), bufferText.end(), bufferText.begin(), ::tolower);
    }

    size_t foundPos = std::string::npos;
    size_t currentPos = gtk_text_iter_get_offset(&searchStart);

    if (forward) {
        // Search forward from current position
        if (currentPos < bufferText.length()) {
            foundPos = bufferText.find(searchText, currentPos);
        }
        // If not found, wrap around to beginning
        if (foundPos == std::string::npos) {
            foundPos = bufferText.find(searchText, 0);
        }
    } else {
        // Search backward from current position
        if (currentPos > 0) {
            std::string beforeCurrent = bufferText.substr(0, currentPos);
            foundPos = beforeCurrent.rfind(searchText);
        }
        // If not found, wrap around to end
        if (foundPos == std::string::npos) {
            foundPos = bufferText.rfind(searchText);
        }
    }

    if (foundPos != std::string::npos) {
        // Update search position
        gtk_text_buffer_get_iter_at_offset(m_editArea->TextViewBuffer, &m_searchStart, foundPos);
        gtk_text_buffer_get_iter_at_offset(m_editArea->TextViewBuffer, &m_searchEnd, foundPos + searchText.length());

        // Select the found text
        gtk_text_buffer_select_range(m_editArea->TextViewBuffer, &m_searchStart, &m_searchEnd);
        
        // Scroll to the found text
        gtk_text_view_scroll_to_iter(m_editArea->TextView, &m_searchStart, 0.1, FALSE, 0.0, 0.0);
        
        // Highlight the match
        HighlightMatch(m_searchStart, m_searchEnd);
        
        UpdateStatus("Found match");
        return true;
    } else {
        UpdateStatus("No matches found");
        return false;
    }
}

void SearchReplaceDialog::HighlightMatch(const GtkTextIter& start, const GtkTextIter& end) {
    ClearHighlight();
    
    // Create a tag for highlighting
    GtkTextTag* tag = gtk_text_buffer_create_tag(m_editArea->TextViewBuffer, "search_highlight",
                                                "background", "yellow",
                                                "foreground", "black",
                                                NULL);
    
    gtk_text_buffer_apply_tag(m_editArea->TextViewBuffer, tag, &start, &end);
}

void SearchReplaceDialog::ClearHighlight() {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(m_editArea->TextViewBuffer, &start, &end);
    gtk_text_buffer_remove_tag_by_name(m_editArea->TextViewBuffer, "search_highlight", &start, &end);
}

void SearchReplaceDialog::UpdateStatus(const std::string& message) {
    gtk_label_set_text(m_statusLabel, message.c_str());
}

void SearchReplaceDialog::ReplaceCurrent() {
    if (gtk_text_buffer_get_has_selection(m_editArea->TextViewBuffer)) {
        const char* replacement = gtk_editable_get_text(GTK_EDITABLE(m_replaceEntry));
        if (replacement) {
            gtk_text_buffer_delete_selection(m_editArea->TextViewBuffer, TRUE, TRUE);
            gtk_text_buffer_insert_at_cursor(m_editArea->TextViewBuffer, replacement, -1);
            UpdateStatus("Replaced");
        }
    } else {
        UpdateStatus("No selection to replace");
    }
}

void SearchReplaceDialog::ReplaceAll() {
    const char* searchText = gtk_editable_get_text(GTK_EDITABLE(m_searchEntry));
    const char* replacement = gtk_editable_get_text(GTK_EDITABLE(m_replaceEntry));
    
    if (!searchText || strlen(searchText) == 0) {
        UpdateStatus("Please enter search text");
        return;
    }

    if (!replacement) {
        replacement = "";
    }

    bool caseSensitive = gtk_check_button_get_active(m_caseSensitiveCheck);
    int replaceCount = 0;

    // Get the full text
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(m_editArea->TextViewBuffer, &start, &end);
    std::string bufferText = gtk_text_buffer_get_text(m_editArea->TextViewBuffer, &start, &end, TRUE);

    // Prepare search text
    std::string searchStr = searchText;
    if (!caseSensitive) {
        std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);
        std::transform(bufferText.begin(), bufferText.end(), bufferText.begin(), ::tolower);
    }

    // Find and replace all occurrences
    size_t pos = 0;
    while ((pos = bufferText.find(searchStr, pos)) != std::string::npos) {
        // Get the original text positions
        GtkTextIter iterStart, iterEnd;
        gtk_text_buffer_get_iter_at_offset(m_editArea->TextViewBuffer, &iterStart, pos);
        gtk_text_buffer_get_iter_at_offset(m_editArea->TextViewBuffer, &iterEnd, pos + searchStr.length());

        // Replace the text
        gtk_text_buffer_delete(m_editArea->TextViewBuffer, &iterStart, &iterEnd);
        gtk_text_buffer_insert(m_editArea->TextViewBuffer, &iterStart, replacement, -1);

        // Update buffer text for next search
        bufferText = gtk_text_buffer_get_text(m_editArea->TextViewBuffer, &start, &end, TRUE);
        if (!caseSensitive) {
            std::transform(bufferText.begin(), bufferText.end(), bufferText.begin(), ::tolower);
        }

        replaceCount++;
        pos += strlen(replacement);
    }

    UpdateStatus("Replaced " + std::to_string(replaceCount) + " occurrences");
}

// Static callback functions
void SearchReplaceDialog::OnFindClicked(GtkButton* button, SearchReplaceDialog* dialog) {
    dialog->FindNext();
}

void SearchReplaceDialog::OnReplaceClicked(GtkButton* button, SearchReplaceDialog* dialog) {
    dialog->ReplaceCurrent();
}

void SearchReplaceDialog::OnReplaceAllClicked(GtkButton* button, SearchReplaceDialog* dialog) {
    dialog->ReplaceAll();
}

void SearchReplaceDialog::OnCloseClicked(GtkButton* button, SearchReplaceDialog* dialog) {
    dialog->Hide();
}

void SearchReplaceDialog::OnSearchEntryActivated(GtkEntry* entry, SearchReplaceDialog* dialog) {
    dialog->FindNext();
}

void SearchReplaceDialog::OnReplaceEntryActivated(GtkEntry* entry, SearchReplaceDialog* dialog) {
    dialog->ReplaceCurrent();
}

void SearchReplaceDialog::OnKeyPressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state, SearchReplaceDialog* dialog) {
    if (keyval == GDK_KEY_Escape) {
        dialog->Hide();
    } else if (keyval == GDK_KEY_F3) {
        if (state & GDK_SHIFT_MASK) {
            dialog->FindPrevious();
        } else {
            dialog->FindNext();
        }
    }
}

void SearchReplaceDialog::OnDialogResponse(GtkDialog* dialog, int response_id, SearchReplaceDialog* self) {
    if (response_id == GTK_RESPONSE_CLOSE || response_id == GTK_RESPONSE_DELETE_EVENT) {
        self->Hide();
    }
}
