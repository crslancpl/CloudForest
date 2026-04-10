#include "LspPopovers_if.h"

#include "LspPopovers.h"
#include "EditArea.h"
#include "datatypes/lsp.h"

static SuggestionPopover* ative_suggestion_popover;
static TipPopover* ActiveTipPopover;

namespace lsppopovers{
void Construct(){
    ative_suggestion_popover = new SuggestionPopover();
    ActiveTipPopover = new TipPopover();
}

void suggestion::SetTargetEditArea(EditArea* target){
    ative_suggestion_popover->setTargetBuffer(target->GetTextBuffer());
    ative_suggestion_popover->setCursorRect(target->GetCursorRectangle());
}

void suggestion::Show(){
    ative_suggestion_popover->Show();
}

void suggestion::Hide(){
    ative_suggestion_popover->Hide();
}

void suggestion::Add(datatypes::Suggestion *item){
    ative_suggestion_popover->Add(item);
}

void suggestion::Clear(){
    ative_suggestion_popover->Clear();
}

void tip::SetTargetEditArea(EditArea* target){
    ActiveTipPopover->setCursorRect(target->GetCursorRectangle());
}

void tip::Show(const char* content){
    ActiveTipPopover->ShowContent(content);
}
void tip::Hide(){
    ActiveTipPopover->Hide();
}

}
