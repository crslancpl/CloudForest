#include "cloudforest_mod_Py.h"

#include "python_tool.h"

#include "language_mod_Py.h"
#include "editarea/editarea_mod_Py.h"
#include "settingpanel/setting_panel_mod_Py.h"


#include <floatobject.h>
#include <listobject.h>
#include <methodobject.h>
#include <object.h>
#include <pylifecycle.h>
#include <pystate.h>
#include <pytypedefs.h>
#include <tupleobject.h>

//callback lists
static PyObject *cloudforest_module_app_closed_callbacks;

static PyObject *cloudforest_module_test(PyObject *self, PyObject *args){
    printf("this is from CloudForest module\n");
    Py_RETURN_NONE;
}

static PyObject* cloudforest_module_add_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callbackfunc;

    if(!PyArg_ParseTuple(args, "sO", &event, &callbackfunc)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callbackfunc)){
        Py_RETURN_NAN;
    }

    if (strcmp(event, "app-closed") == 0) {
        PyList_Append(cloudforest_module_app_closed_callbacks, callbackfunc);
    }
    Py_RETURN_NONE;
}

/*
static PyObject* cloudforest_module_run_extension(PyObject* self, PyObject* args){
    char* entryfile;

    if(!PyArg_ParseTuple(args, "s", &entryfile)){
        Py_RETURN_NONE;
    }

    PyThreadState* ts = execute_extension(entryfile);
    Py_RETURN_NONE;
}
*/

static PyMethodDef cloudforest_module_method[] = {
    {"test",  cloudforest_module_test, METH_VARARGS,"test if module available"},
    {"add_callback",  cloudforest_module_add_callback, METH_VARARGS,"add callback"},
    //{"run_extension", cloudforest_module_run_extension, METH_VARARGS, "run an extension in sub-interpreter"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef cloudforest_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cloudforest",
    .m_size = 0,
    .m_methods = cloudforest_module_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_cloudforest_module(){
    PyObject *cfmodule = PyModule_Create(&cloudforest_module);
    cloudforest_module_app_closed_callbacks = PyList_New(0);
    PyModule_AddObject(cfmodule, "editarea", (PyObject*)PyInit_editarea_module());
    PyModule_AddObject(cfmodule, "setting_panel", (PyObject*)PyInit_setting_panel_module());
    PyModule_AddObject(cfmodule, "language", (PyObject*)PyInit_language_module());
    return cfmodule;
}

void cloudforest_module_invoke_app_closed(){
    printf("cpp: cloudforest_module app closed\n");
    PyObject *args = PyTuple_Pack(0);
    RunCallback(cloudforest_module_app_closed_callbacks, args);
    Py_DECREF(args);
}
