#include "editarea_mod_Py.h"

#include "datatypes/common.h"
#include "editarea_class_Py.h"
#include "pythonbackend/python_tool.h"
#include "src/filemanagement/FileOperation.h"
#include "src/gui/editarea/EditArea.h"
#include "src/session/EditAreaData.h"
#include "src/session/FileData.h"
#include "src/session/SessionEvent.h"
#include "datatypes/file.h"
#include "toolset/event/Event.h"

#include <cstdio>
#include <floatobject.h>
#include <longobject.h>
#include <methodobject.h>
#include <object.h>
#include <unicodeobject.h>
#include <unordered_map>


#define EVENT_NEW_EDITAREA "new-editarea"
#define EVENT_LANGUAGE_CHANGED "language-changed"

/*
 * Module
 */

// lists
static PyObject *registered_editareas = nullptr;

static PythonEventMap event_map = {
    {EVENT_LANGUAGE_CHANGED, PythonEvent()},
    {EVENT_NEW_EDITAREA, PythonEvent()}
};

py_EditArea* find_editarea_py(const EditArea *ea){
    if (registered_editareas == nullptr) {
        return nullptr;
    }

    for(size_t itr = 0; itr < PyList_GET_SIZE(registered_editareas); itr++){
        py_EditArea *registeredea = (py_EditArea*)PyList_GET_ITEM(registered_editareas, itr);
        if(registeredea->editarea == ea){
            return registeredea;
        }
    }
    return nullptr;
}

static PyObject *editarea_module_add_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callback;
    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        return nullptr;
    }
    if(!PyCallable_Check(callback)){
        return nullptr;
    }

    auto itr = event_map.find(event);
    if (itr != event_map.end()){
        itr->second.Connect(callback);
    }

    Py_RETURN_NONE;
}

static PyObject *editarea_module_find_by_file_path(PyObject *self, PyObject *args){
    char* filepath;

    if(!PyArg_ParseTuple(args, "s", &filepath)){
        Py_RETURN_NAN;
    }

    EditArea* ea = session::FindEditAreaByPath(filepath);
    if(!ea){
        Py_RETURN_NONE;
    }
    PyObject* ea_py = (PyObject*)find_editarea_py(ea);
    return  ea_py;
}

static PyObject* editarea_module_find_workspace_path(PyObject* self, PyObject* args){
    py_EditArea* ea;
    if(!PyArg_ParseTuple(args, "O", &ea)){
        Py_RETURN_NAN;
    }

    Workspace* ws = session::FindWorkspaceByPath(ea->editarea->GetFilePath());
    if(ws){
        return PyUnicode_FromString(ws->GetFileData()->absoPath);
    }else{
        Py_RETURN_NONE;
    }
}

static PyMethodDef editarea_module_methods[]={
    {"add_callback", editarea_module_add_callback, METH_VARARGS, "add callback for event"},
    {"find_by_file_path", editarea_module_find_by_file_path, METH_VARARGS, "find EditArea object with file path"},
    {"find_workspace_path", editarea_module_find_workspace_path, METH_VARARGS, "find the path of the editarea's workspace"},
    {nullptr, nullptr, 0, nullptr}
};

static PyModuleDef editarea_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "editarea",
    .m_size = 0,
    .m_methods = editarea_module_methods,
    .m_slots = nullptr,
};

void editarea_py_register(EditArea *ea){
    /*
     * When the gui creates a EditArea, we have to create an
     * instance of the cf_EditArea, so the python extension can
     * manipulate the edit area
     */
    RestoreThreadLock();
    py_EditArea *newEa = py_EditArea_create_object();

    newEa->editarea = ea;
    newEa->filePath = strdup(ea->GetFilePath());

    py_EditArea_connect_events(newEa);
    // we are trying to make filepath a variable
    PyObject *filepath = PyUnicode_FromString(ea->GetFilePath());
    PyObject *args = PyTuple_Pack(1, newEa);
    PyList_Append(registered_editareas, (PyObject*)newEa);

    PythonEvent &event = event_map.at(EVENT_NEW_EDITAREA);
    event.Invoke(args);

    Py_DECREF(filepath);
    Py_DECREF(args);

    ReleaseThreadLock();
}

PyMODINIT_FUNC PyInit_editarea_module(){
    registered_editareas = PyList_New(0);

    PyObject *eamodule = PyModule_Create(&editarea_module);

    PyModule_AddObject(eamodule, "EditArea", (PyObject*)PyInit_py_EditArea_class());
    session::Listen(session::EDITAREA_CREATED, (EventCallback)editarea_py_register);
    return eamodule;
}
