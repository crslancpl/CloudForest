#include "python_if.h"


#include "cloudforest_mod_Py.h"
#include "python_tool.h"

#include <Python.h>
#include <string>


#define PY_SSIZE_T_CLEAN

void pybackend::Start(){
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
    ExecuteFile("data/extension/init.py");

    return;

exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}

void pybackend::End(){
    cloudforest_module_invoke_app_closed();
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
}
