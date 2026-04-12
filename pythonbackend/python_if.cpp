#include "python_if.h"

#include <Python.h>
#include <string>

#include "cloudforest_mod_Py.h"

#define PY_SSIZE_T_CLEAN

static void Execute(const std::string &code){
    PyRun_SimpleString(code.c_str());
}

static void ExecuteFile(const std::string &path){
    FILE *f = fopen(path.c_str(),"r");
    PyRun_SimpleFile(f, path.c_str());
}

void pybackend::Start(){
    int result = PyImport_AppendInittab("cloudforest", PyInit_cloudforest_module);

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
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
}
