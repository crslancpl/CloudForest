#include "EditorsManager.h"

#include "editarea/EditArea.h"
#include "layouts/tab/CfTabLayout.h"
#include "layouts/tab/CfTabSwitcher.h"
#include "src/AppContext.h"
#include "src/filemanagement/FileTree.h"
#include "src/session/TabManager.h"
#include "toolset/event/Event.h"

#include <memory>



typedef void (*EditAreaCreatedCallback)(EditArea*);

EditorsManager::EditorsManager(AppContext& context) :
    m_context(context)
{
    //
}

EditorsManager::~EditorsManager(){}

EditArea& EditorsManager::EditNewFile(){
    FileBranch& file = m_context.fileManager->CreateNewFile();
    return this->CreateEditArea(file);
}

EditArea& EditorsManager::EditFile(FileBranch& file){
    EditArea* ea = file.GetEditArea();

    if (!ea) {
        ea = &this->CreateEditArea(file);
        file.SetEditArea(ea);
    }

    m_focused = ea;
    EditArea& ref = *ea;

    return ref;
}

EditArea* EditorsManager::FindEditAreaByPath(const std::string& path){
    FileBranch* b = m_context.fileManager->FindFileByPath(path);
    if (b) {
        return b->GetEditArea();
    }

    return nullptr;
}


/*
 * Private
 */

EditArea& EditorsManager::CreateEditArea(FileBranch& file){
    std::unique_ptr<EditArea> ea = std::make_unique<EditArea>(file, m_context);
    EditArea& ref = *ea;

    EEditAreaCreated.Invoke(ea.get());

    m_context.sessionManager->GetTabManager().GetFocusedTabLayout()->Add(std::move(ea));
    return ref;
}
