#include "CompletionTool.h"



#include <gtk/gtk.h>
#include <memory>

CompletionPopoverItem::CompletionPopoverItem(const Completion& completion){
    m_button = GTK_BUTTON(gtk_button_new());
}

CompletionPopoverItem::~CompletionPopoverItem(){
    //
}

CompletionPopover::CompletionPopover(){
    //
}

CompletionPopover::~CompletionPopover(){
    //
}

CompletionTool::CompletionTool(EditArea& parent):
    m_parent(parent)
    {
    //
    m_completionPopover = std::make_unique<CompletionPopover>();
}

CompletionTool::~CompletionTool(){
    //
}

void CompletionTool::Request(unsigned int line, unsigned int column){
    //
}

void CompletionTool::New(std::unique_ptr<Completion> completion){
    m_completionsList.push_back(std::move(completion));
}
