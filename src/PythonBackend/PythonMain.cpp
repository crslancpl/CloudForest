#include "PythonMain.h"
#include <python3.10/cpython/initconfig.h>



void PyBackend::Start(){
    PyStatus status;
    PyConfig config;
    PyConfig_InitPythonConfig(&config);

    status = PyConfig_SetBytesString(&config, &config.program_name, NULL);
    if (PyStatus_Exception(status)){
        goto exception;
    }
    PyConfig_Clear(&config);

    exception:
    PyConfig_Clear(&config);
    Py_ExitStatusException(status);
}

void PyBackend::Execute(const string &code){
    PyRun_SimpleString(code.c_str());
}

void PyBackend::ExecuteFile(const string &path){
    //PyRun_SimpleFile(FILE *f, const char *p);
}
