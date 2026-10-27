#include "cloudforest_mod_Py.h"

#include "python_tool.h"
#include "language/language_mod_Py.h"
#include "pythonbackend/python_if.h"
#include "setting_mod_Py.h"
#include "editarea/editarea_mod_Py.h"
#include "src/AppContext.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileManager.h"

#include <cstdio>
#include <abstract.h>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>
#include <memory>
#include <methodobject.h>
#include <modsupport.h>
#include <moduleobject.h>
#include <object.h>
#include <pylifecycle.h>
#include <pystate.h>
#include <pytypedefs.h>
#include <string>
#include <tupleobject.h>
#include <unicodeobject.h>

#define EVENT_APP_CLOSED "app-closed"
#define EVENT_NEW_WORKSPACE "new-workspace"

//callback lists

static PythonEventContainer events;

static void on_new_workspace(Workspace* ws){
    TryRestoreThreadLock();

    PythonEvent* event = events.Find(EVENT_NEW_WORKSPACE);
    if (event) {
        PyObject* args = PyTuple_Pack(
            2,
            PyUnicode_FromString(ws->GetFileData()->fileName.c_str()),
            PyUnicode_FromString(ws->GetFileData()->absoPath.c_str())
        );

        event->Invoke(args);
        Py_DECREF(args);
    }

    ReleaseThreadLock();
}


/*
 * Module function
 */

static PyObject *cloudforest_module_test(PyObject *self, PyObject *args){
    printf("this is from CloudForest module\n");
    Py_RETURN_NONE;
}

static PyObject *cloudforest_module_get_workspaces(PyObject *self, PyObject *args){
    /*
     * get_workspace function will return a list of {name, uri}
     *
     * [usage]
     * cloudforest.get_workspaces()
     * [return]
     * [ {name1, uri1}, {name2, uri2}, ...]
     */

    PyObject* wslist = PyList_New(0);
    for(const std::unique_ptr<Workspace>& ws : pybackend::GetAppContext().fileManager->GetWorkspaces()){
        PyObject* wsproperty = PyDict_New();
        PyDict_SetItemString(wsproperty, "name", PyUnicode_FromString(ws->GetFileData()->fileName.c_str()));
        std::string uri = "file://" + std::string(ws->GetFileData()->absoPath);
        PyDict_SetItemString(wsproperty, "uri", PyUnicode_FromString(uri.c_str()));

        PyList_Append(wslist, wsproperty);
    }
    return wslist;
}

static PyObject* cloudforest_module_connect(PyObject *self, PyObject *args){
    char* signal;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &signal, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* event = events.Find(signal);
    if (event) {
        printf("signal %s\n", signal);
        event->Connect(callback);
    }

    Py_RETURN_NONE;
}


static PyMethodDef cloudforest_module_method[] = {
    {"test",  cloudforest_module_test, METH_VARARGS,"test if module available"},
    {"get_workspaces", cloudforest_module_get_workspaces, METH_VARARGS, "iterate through every workspaces and call the callback"},
    {"connect",  cloudforest_module_connect, METH_VARARGS,"add callback"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef_Slot cloudforest_module_slot[] = {
    {0, nullptr}
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

    PyModule_AddObject(cfmodule, "editarea", (PyObject*)PyInit_editarea_module());
    PyModule_AddObject(cfmodule, "language", (PyObject*)PyInit_language_module());
    PyModule_AddObject(cfmodule, "setting", (PyObject*)PyInit_setting_module());

    pybackend::GetAppContext().fileManager->EWorkspaceCreated.Connect(on_new_workspace);

    return cfmodule;
}

void cloudforest_module_invoke_app_closed(){
    printf("cpp: cloudforest_module app closed\n");

    PythonEvent* event = events.Find(EVENT_APP_CLOSED);

    if (event) {
        PyObject *args = PyTuple_Pack(0);
        event->Invoke(args);
        Py_DECREF(args);
    }
}
