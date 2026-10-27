#include "SessionManager.h"

#include "editarea/CompletionTool.h"
#include "src/session/EditorsManager.h"
#include "src/session/ExtensionManager.h"
#include "src/session/TabManager.h"

#include <memory>

SessionManager::SessionManager(AppContext& context)
{
    m_editorsManager = std::make_unique<EditorsManager>(context);
    m_extensionManager = std::make_unique<ExtensionManager>();
    m_tabManager = std::make_unique<TabManager>(context);
}

SessionManager::~SessionManager(){
    //
}

EditorsManager& SessionManager::GetEditorsManager() const{
    return *m_editorsManager;
}

ExtensionManager& SessionManager::GetExtensionManager() const{
    return *m_extensionManager;
}

TabManager& SessionManager::GetTabManager() const{
    return *m_tabManager;
}
