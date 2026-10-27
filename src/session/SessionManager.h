#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include "EditorsManager.h"
#include "src/session/EditorsManager.h"
#include "src/session/ExtensionManager.h"
#include "src/session/TabManager.h"

#include <memory>

// forward declare
typedef struct AppContext AppContext;

class SessionManager {
public:
    SessionManager(AppContext& context);
    ~SessionManager();

    EditorsManager& GetEditorsManager() const;
    ExtensionManager& GetExtensionManager() const;
    TabManager& GetTabManager() const;

    void Save();

private:
    std::unique_ptr<EditorsManager> m_editorsManager;
    std::unique_ptr<ExtensionManager> m_extensionManager;
    std::unique_ptr<TabManager> m_tabManager;
};

#endif
