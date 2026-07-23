#include "CompletionTool.h"

#include "EditArea.h"
#include "CompletionPopover.h"
#include "datatypes/lsp.h"
#include "Gui_if.h"

#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

/*
 * CompletionTool
 */

// static

std::unordered_set<char> CompletionTool::default_skip_char = {
    '[',
    ']',
    '(',
    ')',
    '{',
    '}',
    '"',
    '\'',
    ' ',
    '\t',
    '=',
    ',',
    ';'
};

bool CompletionTool::IsCompletionTriggerChar(char c){
    auto itr = default_skip_char.find(c);
    if (itr != default_skip_char.end()) {
        return false;
    }
    return true;
}


// non static

CompletionTool::CompletionTool(EditArea& parent):
    m_parent(parent)
    {
    //
}

CompletionTool::~CompletionTool(){
    //
    if (m_completionPopover) {
        gui::TransferCompletionPopover(nullptr);
    }
}

void CompletionTool::Add(std::unique_ptr<Completion> completion){
    m_completionsList.push_back(std::move(completion));
}

void CompletionTool::Clear(){
    //
    if (m_completionPopover) {
        m_completionsList.clear();
    }
}

void CompletionTool::HidePopover(){
    if (m_completionPopover) {
        m_completionPopover->Hide();
        m_isShowing = false;
    }
}

void CompletionTool::Request(unsigned int line, unsigned int column){
    //
}

void CompletionTool::ShowPopover(){
    //
    if (!m_completionPopover) {
        gui::TransferCompletionPopover(this);
    }

    m_completionPopover->SetTarget(&m_parent);
    m_completionPopover->Show(m_parent.GetCursorRectangle(), m_completionsList);
    m_isShowing = true;
}

void CompletionTool::SelectUp(){
    m_completionPopover->SelectUp();
}

void CompletionTool::SelectDown(){
    m_completionPopover->SelectDown();
}

void CompletionTool::Confirm(){
    const Completion& result = m_completionPopover->Confirm();
    m_parent.Replace(result.range, result.insertText.c_str());
}

bool CompletionTool::GetIsShowing(){
    return m_isShowing;
}

std::unique_ptr<CompletionPopover> CompletionTool::GetPopoverOwnership(){
    m_completionPopover->Hide();
    m_isShowing = false;
    return std::move(m_completionPopover);
}

void CompletionTool::SetPopoverOwnership(std::unique_ptr<CompletionPopover> popover){
    m_completionPopover = std::move(popover);
}
