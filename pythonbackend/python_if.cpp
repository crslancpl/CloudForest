#include "python_if.h"


#include "cloudforest_mod_Py.h"
#include "datatypes/extension.h"
#include "python_tool.h"
#include "src/session/ExtensionManager.h"
#include "src/session/SessionManager.h"
#include "src/AppContext.h"

#include <Python.h>
#include <ceval.h>
#include <cstdio>
#include <import.h>
#include <memory>
#include <pystate.h>
#include <string>
#include <vector>


#define PY_SSIZE_T_CLEAN

static AppContext* current_context;

namespace pybackend{

void Start(AppContext& context){
    current_context = &context;
    PyImport_AppendInittab("cloudforest", PyInit_cloudforest_module);

    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    status = PyConfig_SetBytesString(&config, &config.program_name, nullptr);
    if (PyStatus_Exception(status)) {
        goto exception;
    }

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    PyConfig_Clear(&config);

    ExecuteFile("pythonscripts/init.py");
    ExecuteFile("pythonscripts/entry.py");

    ReleaseThreadLock();
    return;

exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}


void End(){
    if (!TryRestoreThreadLock()) {
        return;
    }
    cloudforest_module_invoke_app_closed();

    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
}

AppContext& GetAppContext(){
    return *current_context;
}

void RunEnabledExtensions(){
    if (!TryRestoreThreadLock()) {
        return;
    }

    ExtensionManager& manager = pybackend::GetAppContext().sessionManager->GetExtensionManager();
    const std::vector<std::unique_ptr<Extension>>& extensions = manager.GetAllExtensions();

    for(const std::unique_ptr<Extension>& ext : extensions){
        if(ext->enabled){
            ext->module = PyImport_ImportModule(ext->folder);
        }
    }
    ReleaseThreadLock();
}

}// namespace pybackend
