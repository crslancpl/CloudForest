#ifndef APPCONTEXT_H_
#define APPCONTEXT_H_

#include "AppUI.h"
#include "src/filemanagement/FileManager.h"
#include "src/languages/LanguageManager.h"
#include "src/session/SessionManager.h"

#include <memory>

typedef struct AppContext {
    std::unique_ptr<AppUI> appUI;
    std::unique_ptr<FileManager> fileManager;
    std::unique_ptr<LanguageManager> langManager;
    std::unique_ptr<SessionManager> sessionManager;
} AppContext;

#endif
