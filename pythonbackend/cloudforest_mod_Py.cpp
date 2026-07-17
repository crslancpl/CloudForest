#include "cloudforest_mod_Py.h"

#include "python_tool.h"
#include "language_mod_Py.h"
#include "setting_mod_Py.h"
#include "editarea/editarea_mod_Py.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileOperation.h"
#include "src/session/SessionEvent.h"
#include "src/session/FileData.h"

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
#include <unordered_map>

#define EVENT_APP_CLOSED "app-closed"
#define EVENT_NEW_WORKSPACE "new-workspace"

//callback lists

static std::unordered_map<std::string, PythonEvent> event_map= {
    {EVENT_APP_CLOSED, PythonEvent()},
    {EVENT_NEW_WORKSPACE, PythonEvent()}
};

static void OnNewWorkspace(Workspace* ws){
    RestoreThreadLock();
    PyObject* args = PyTuple_Pack(2, PyUnicode_FromString(ws->GetName()), PyUnicode_FromString(ws->GetFileData()->absoPath));
    PythonEvent &event = event_map.at(EVENT_NEW_WORKSPACE);
    event.Invoke(args);
    Py_DECREF(args);
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
    for(const std::unique_ptr<Workspace>& ws : session::GetWorkspaceList()){
        PyObject* wsproperty = PyDict_New();
        PyDict_SetItemString(wsproperty, "name", PyUnicode_FromString(ws->GetName()));
        std::string uri = "file://" + std::string(ws->GetFileData()->absoPath);
        PyDict_SetItemString(wsproperty, "uri", PyUnicode_FromString(uri.c_str()));

        PyList_Append(wslist, wsproperty);
    }
    return wslist;
}

static PyObject* cloudforest_module_add_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    auto itr = event_map.find(event);
    if (itr != event_map.end()){
        itr->second.Connect(callback);
    }

    Py_RETURN_NONE;
}


static PyMethodDef cloudforest_module_method[] = {
    {"test",  cloudforest_module_test, METH_VARARGS,"test if module available"},
    {"get_workspaces", cloudforest_module_get_workspaces, METH_VARARGS, "iterate through every workspaces and call the callback"},
    {"add_callback",  cloudforest_module_add_callback, METH_VARARGS,"add callback"},
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

    session::Listen(session::NEW_WORKSPACE, (EventCallback)OnNewWorkspace);
    return cfmodule;
}

void cloudforest_module_invoke_app_closed(){
    printf("cpp: cloudforest_module app closed\n");
    PyObject *args = PyTuple_Pack(0);
    PythonEvent &event = event_map.at(EVENT_APP_CLOSED);
    event.Invoke(args);
    Py_DECREF(args);
}
