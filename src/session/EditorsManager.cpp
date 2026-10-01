#include "EditorsManager.h"

#include "editarea/EditArea.h"
#include "layouts/tab/CfTabLayout.h"
#include "layouts/tab/CfTabSwitcher.h"
#include "src/AppContext.h"
#include "src/filemanagement/FileTree.h"
#include "src/session/TabData.h"
#include <memory>

EditorsManager::EditorsManager(AppContext& context) :
    m_context(context)
{
    //
}

EditorsManager::~EditorsManager(){}

EditArea& EditorsManager::EditNewFile(){
    FileBranch& file = m_context.fileManager->CreateNewFile();
    std::unique_ptr<EditArea> newea = std::make_unique<EditArea>(file, m_context);
    EditArea& ref = *newea;
    session::GetFocusedTabLayout()->Add(std::move(newea));
    return ref;
}

EditArea& EditorsManager::EditFile(FileBranch& file){
    CfTabLayout* tablayout = session::GetFocusedTabLayout();
    if (!tablayout) {
        //return ;
    }

    EditArea* ea = file.GetEditArea();

    if (!ea) {
        //
        std::unique_ptr<EditArea> newea = std::make_unique<EditArea>(file, m_context);
        ea = newea.get();
        CfTabSwitcher& switcher = tablayout->Add(std::move(newea));
        tablayout->SwitchTo(switcher);
    }

    m_focused = ea;
    EditArea& ref = *ea;

    return ref;
}
