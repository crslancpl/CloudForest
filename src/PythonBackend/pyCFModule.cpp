#include "pyCFModule.h"

#include <Python.h>
#include <abstract.h>
#include <cstdio>
#include <cstring>
#include <listobject.h>
#include <methodobject.h>
#include <modsupport.h>
#include <object.h>
#include <future>

#include "pyEditArea.h"

#include "../Core.h"
#include "../Global.h"
#include "../Types.h"
#include "../Classes.h"

static PyObject *error;

static int cf_module_exec(PyObject *m){
    if (error != NULL) {
            PyErr_SetString(PyExc_ImportError,
                            "cannot initialize cf module more than once");
            return -1;
        }
        error = PyErr_NewException("cf.error", NULL, NULL);
        if (PyModule_AddObjectRef(m, "cfError", error) < 0) {
            return -1;
        }
    return 0;
}


static PyObject *cf_Test(PyObject *self, PyObject *args){
    printf("tested module\n");
    Py_RETURN_NONE;
}


static PyObject *OpenEditAreaCallbackList = nullptr;

static PyObject *cf_Add_Callback(PyObject *self, PyObject *args){
    PyObject* callback;
    char* type;

    /*
     * [!NOTE]
     * type should be "OPENEDITAREA"
     */
    if(!PyArg_ParseTuple(args,"s0",&type,&callback)){
        return nullptr;
    }
    if(!PyCallable_Check(callback)){
        printf("error: cf add callback argument is not callable\n");
    }

    PyObject *callbacklist;

    if(std::strcmp(type, "OPENEDITAREA") == 0){
        callbacklist = OpenEditAreaCallbackList;
    }

    PyList_Append(OpenEditAreaCallbackList, callback);

    Py_INCREF(callback);

    Py_RETURN_NONE;
}

static PyObject *OpenLanguageServerFunc = nullptr;
static PyObject *cf_NewLanguageServer(PyObject *self, PyObject *args){
    PyObject *func;
    if(!PyArg_ParseTuple(args, "O", func)){
        return nullptr;
    }

    if(!PyCallable_Check(func)){
        return nullptr;
    }

    OpenLanguageServerFunc = func;

    Py_RETURN_NONE;
}

static PyMethodDef cf_method[] ={
    {"test",  cf_Test, METH_VARARGS,"test if module available"},
    {"AddCallback",  cf_Add_Callback, METH_VARARGS,"add callback to event"},
    {"ListenNewLanguageServerReq", cf_NewLanguageServer, METH_VARARGS, "We don't recommend you to open language server yourself"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef cloudforestmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "CloudForestPy",
    .m_size = 0,
    .m_methods = cf_method,
    .m_slots = nullptr
};


PyMODINIT_FUNC init_cloudforest_module(void){
    OpenEditAreaCallbackList = PyList_New(0);
    OpenLanguageServerFunc = PyList_New(0);
    PyObject *cfmodule = PyModule_Create(&cloudforestmodule);

    PyModule_AddObject(cfmodule, "EditAreaMod", (PyObject*)init_cf_EditArea_module());
    return cfmodule;
}


void cfmod_RunCallback(PyRunCallBack* req){
    PyObject *args;

    if(req->Type == PyRunCallBack::NEWEDITAREA){
        args = Py_BuildValue("(0)", nullptr);
        unsigned int size = PyList_GET_SIZE(OpenEditAreaCallbackList);
        for(unsigned int i = 0; i < size; i++){
            PyObject *func = PyList_GetItem(OpenEditAreaCallbackList, i);
            PyObject_CallObject(func, args);
        }
    }

    Py_DecRef(args);
}

void cfmod_OpenLanguageServer(PyOpenLanguageServer* req){
    PyObject *args;

    args = Py_BuildValue("(ss)", req->LanguageServerCommand, req->CommandOption);

    PyObject_CallObject(OpenLanguageServerFunc, args);

    Py_DecRef(args);
}
