#include "cloudforest_mod_Py.h"

#include "python_tool.h"
#include "language_mod_Py.h"
#include "setting_mod_Py.h"
#include "editarea/editarea_mod_Py.h"
#include "src/filemanagement/FileCallback.h"
#include "src/filemanagement/FileOperation.h"
#include "datatypes/file.h"

#include <cstdio>
#include <abstract.h>
#include <cstring>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>
#include <methodobject.h>
#include <modsupport.h>
#include <moduleobject.h>
#include <object.h>
#include <pylifecycle.h>
#include <pystate.h>
#include <pytypedefs.h>
#include <tupleobject.h>
#include <unicodeobject.h>

//callback lists
static PyObject *cloudforest_module_app_closed_callbacks;
static PyObject *cloudforest_module_new_workspace_callbacks;


static void OnNewWorkspace(Workspace* ws){
    RestoreThreadLock();
    PyObject* args = PyTuple_Pack(2, PyUnicode_FromString(ws->name), PyUnicode_FromString(ws->rootFolderData->absoPath));
    RunCallback(cloudforest_module_new_workspace_callbacks, args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

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
    for(Workspace* ws : filemanagement::GetWorkspaceList()){
        PyObject* wsproperty = PyDict_New();
        PyDict_SetItemString(wsproperty, "name", PyUnicode_FromString(ws->name));
        std::string uri = "file://" + std::string(ws->rootFolderData->absoPath);
        PyDict_SetItemString(wsproperty, "uri", PyUnicode_FromString(uri.c_str()));

        PyList_Append(wslist, wsproperty);
    }
    return wslist;
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
    }else if(strcmp(event, "new-workspace") == 0){
        PyList_Append(cloudforest_module_new_workspace_callbacks, callbackfunc);
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
    cloudforest_module_app_closed_callbacks = PyList_New(0);
    cloudforest_module_new_workspace_callbacks = PyList_New(0);
    PyModule_AddObject(cfmodule, "editarea", (PyObject*)PyInit_editarea_module());
    PyModule_AddObject(cfmodule, "language", (PyObject*)PyInit_language_module());
    PyModule_AddObject(cfmodule, "setting", (PyObject*)PyInit_setting_module());

    filemanagement::ListenEvent(filemanagement::FILE_EVENT_NEW_WORKSPACE, (EventCallback)OnNewWorkspace);
    return cfmodule;
}

void cloudforest_module_invoke_app_closed(){
    printf("cpp: cloudforest_module app closed\n");
    PyObject *args = PyTuple_Pack(0);
    RunCallback(cloudforest_module_app_closed_callbacks, args);
    Py_DECREF(args);
}
