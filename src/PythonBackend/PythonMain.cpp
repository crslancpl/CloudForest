#include "pyEditArea.h"
#include <cstdio>
#include <glib/gprintf.h>
#define PY_SSIZE_T_CLEAN

#include "PythonMain.h"
#include "pyCFModule.h"
#include "cfPythonTool.h"


static void Execute(const string &code){
    PyRun_SimpleString(code.c_str());
}

static void ExecuteFile(const string &path){
    FILE *f = fopen(path.c_str(),"r");
    PyRun_SimpleFile(f, path.c_str());
}

void pybackend::Start(){
    PyImport_AppendInittab("CloudForestPy", init_cloudforest_module);

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

    ExecuteFile("extension/init.py");

    InitPythonTool();
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

const result::Result* pybackend::Process(Request* request){
    if(auto req = dynamic_cast<PyRunCode*>(request)){
        Execute(req->Code);
    }else if(auto req = dynamic_cast<PyRunFile*>(request)){
        ExecuteFile(req->Filepath);
    }else if(auto req = dynamic_cast<PyGetLspMessage*>(request)){
        return GetLspMessage(req);
    }else if(auto req =  dynamic_cast<PyRunCallBack*>(request)){
        cfmod_RunCallback(req);
    }else if(auto req = dynamic_cast<PyRegisterEA*>(request)){
        register_py_EditArea(req);
    }else if(auto req = dynamic_cast<PyCallbackEA*>(request)){
        callback_py_EditArea_object(req);
    }else if(auto req = dynamic_cast<PyOpenLanguageServer*>(request)){
        cfmod_OpenLanguageServer(req);
    }
    return nullptr;
}
