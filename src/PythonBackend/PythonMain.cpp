#include <cstdio>
#define PY_SSIZE_T_CLEAN

#include "PythonMain.h"
#include "cfModule.h"


void pybackend::Start(){
    PyImport_AppendInittab("CloudForest", initcloudforestmodule);

    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);


    /* optional but recommended */
    status = PyConfig_SetBytesString(&config, &config.program_name, nullptr);
    if (PyStatus_Exception(status)) {
        goto exception;
    }

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto exception;
    }
    PyConfig_Clear(&config);


    ExecuteFile("extension/init.py");
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

void pybackend::Execute(const string &code){
    PyRun_SimpleString(code.c_str());
}

void pybackend::ExecuteFile(const string &path){
    FILE *f = fopen(path.c_str(),"r");
    PyRun_SimpleFile(f, path.c_str());
}
