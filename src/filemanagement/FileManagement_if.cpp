#include "FileManagement_if.h"
#include "src/App.h"
#include "src/filemanagement/FileOperation.h"

void filemanager::Init(App& app){
    FileOperationInit(app);
}
