#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include "EditorsManager.h"
#include "src/session/EditorsManager.h"
#include <memory>

// forward declare
typedef struct AppContext AppContext;

class SessionManager {
public:
    SessionManager(AppContext& context);
    ~SessionManager();

    EditorsManager& GetEditorsManager() const;

    void Save();

private:
    std::unique_ptr<EditorsManager> m_editorsManager;

};

#endif
