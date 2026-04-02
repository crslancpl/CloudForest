#include "LspPopovers_if.h"

#include "LspPopovers.h"
#include "EditArea.h"
#include "datatypes/lsp_data.h"

static SuggestionPopover* ActiveSuggestionPopover;
static TipPopover* ActiveTipPopover;

namespace lsppopovers{
void Construct(){
    ActiveSuggestionPopover = new SuggestionPopover();
    ActiveTipPopover = new TipPopover();
}

void suggestion::SetTargetEditArea(EditArea* target){
    ActiveSuggestionPopover->setTargetBuffer(target->getTextBuffer());
    ActiveSuggestionPopover->setCursorRect(target->getCursorRectangle());
}

void suggestion::Show(){
    ActiveSuggestionPopover->Show();
}

void suggestion::Hide(){
    ActiveSuggestionPopover->Hide();
}

void suggestion::Add(datatypes::Suggestion *item){
    ActiveSuggestionPopover->Add(item);
}

void suggestion::Clear(){
    ActiveSuggestionPopover->Clear();
}

void tip::SetTargetEditArea(EditArea* target){
    ActiveTipPopover->setCursorRect(target->getCursorRectangle());
}

void tip::Show(const char* content){
    ActiveTipPopover->ShowContent(content);
}
void tip::Hide(){
    ActiveTipPopover->Hide();
}

}
