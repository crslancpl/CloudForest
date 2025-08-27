#include <cstdio>
#define PY_SSIZE_T_CLEAN

#include "PythonMain.h"
#include <import.h>
#include <modsupport.h>
#include <moduleobject.h>
#include <object.h>
#include <pyport.h>
#include <python3.10/cpython/initconfig.h>


static int cf_module_exec(PyObject *m){
    return 0;
}

static PyModuleDef_Slot cf_module_slots[] ={
    {Py_mod_exec, (void*)cf_module_exec},
    {0,nullptr}
};

static struct PyModuleDef cloudforestmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "CloudForest",
    .m_size = 0,
    .m_slots = cf_module_slots,
};

static PyObject *cloudforestobj(PyObject *self, PyObject *args){
    const char* message;
    printf("asdad\n");
    return 0;
}

PyMODINIT_FUNC initcloudforestmodule(void){
    return PyModule_Create(&cloudforestmodule);
}

void PyBackend::Start(){
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


    PyRun_SimpleString("import CloudForest\n"
                       "print('import')\n");
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    return;

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
