#include "SessionManager.h"

#include "editarea/CompletionTool.h"
#include "src/session/EditorsManager.h"

#include <memory>

SessionManager::SessionManager(AppContext& context)
{
    m_editorsManager = std::make_unique<EditorsManager>(context);
}

SessionManager::~SessionManager(){
    //
}

EditorsManager& SessionManager::GetEditorsManager() const{
    return *m_editorsManager;
}
