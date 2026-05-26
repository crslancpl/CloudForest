#include "editarea_mod_Py.h"

#include "datatypes/common.h"
#include "editarea_class_Py.h"
#include "pythonbackend/python_tool.h"
#include "src/filemanagement/FileOperation.h"
#include "src/gui/editarea/EditArea.h"
#include "src/gui/editarea/EditArea_if.h"
#include "datatypes/file.h"

#include <cstdio>
#include <floatobject.h>
#include <longobject.h>
#include <methodobject.h>
#include <object.h>
#include <unicodeobject.h>


/*
 * Module
 */

// lists
static PyObject *registered_editareas = nullptr;
static PyObject *ea_registered_callback_list = nullptr;
static PyObject *ea_language_changed_callback_list = nullptr;

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

void editarea_py_invoke_text_changed(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = find_editarea_py(ea);
    if(py_ea == nullptr) {
        printf("cannot find ea\n");
        return;
    }
    Py_INCREF(py_ea);
    const Difference &dif = ea->GetPendingDiff();
    PyObject* args = PyTuple_Pack(4, py_ea, GetPyDictFromZRange(dif.before), PyUnicode_FromString(dif.text), PyLong_FromLong(ea->GetFileVersion()));
    RunCallback(py_ea->textChangedCallbacks, args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

void editarea_py_invoke_lang_changed(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = find_editarea_py(ea);
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->langChangedCallbacks, args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

void editarea_py_invoke_cursor_moved(EditArea *ea, const ZPosition &pos){
    RestoreThreadLock();
    py_EditArea* py_ea = find_editarea_py(ea);
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(3, py_ea, PyLong_FromLong(pos.line), PyLong_FromLong(pos.column));
    RunCallback(py_ea->cursorMovedCallbacks, args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

void editarea_py_invoke_completion_requested(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = find_editarea_py(ea);
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->completionRequestedCallbacks, args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

void editarea_py_invoke_file_saved(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = find_editarea_py(ea);
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->fileSavedCallbacks, args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

void editarea_py_invoke_filedata_changed(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = find_editarea_py(ea);
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->fileDataChangedCallbacks, args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

static PyObject *editarea_module_add_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callbackfunc;
    if(!PyArg_ParseTuple(args, "sO", &event, &callbackfunc)){
        return nullptr;
    }
    if(!PyCallable_Check(callbackfunc)){
        return nullptr;
    }

    if (strcmp(event, "new-editarea") == 0) {
        PyList_Append(ea_registered_callback_list, callbackfunc);
    } else if(strcmp(event, "language-changed") == 0){
        PyList_Append(ea_language_changed_callback_list, callbackfunc);
    }

    Py_RETURN_NONE;
}

static PyObject *editarea_module_find_by_file_path(PyObject *self, PyObject *args){
    char* filepath;

    if(!PyArg_ParseTuple(args, "s", &filepath)){
        Py_RETURN_NAN;
    }

    EditArea* ea = editarea::FindEditArea(filepath);
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

    Workspace* ws = filemanagement::FindWorkspaceFromPath(ea->editarea->GetFilePath());
    if(ws){
        return PyUnicode_FromString(ws->rootFolderData->absoPath);
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

    // we are trying to make filepath a variable
    PyObject *filepath = PyUnicode_FromString(ea->GetFilePath());
    PyObject *args = PyTuple_Pack(1, newEa);
    PyList_Append(registered_editareas, (PyObject*)newEa);
    RunCallback(ea_registered_callback_list, args);

    Py_DECREF(filepath);
    Py_DECREF(args);

    ReleaseThreadLock();
}

PyMODINIT_FUNC PyInit_editarea_module(){
    registered_editareas = PyList_New(0);
    ea_registered_callback_list = PyList_New(0);
    ea_language_changed_callback_list = PyList_New(0);

    PyObject *eamodule = PyModule_Create(&editarea_module);

    PyModule_AddObject(eamodule, "EditArea", (PyObject*)PyInit_py_EditArea_class());
    editarea::ListenEvent(editarea::EDITAREA_CREATED, (void (*)())editarea_py_register);
    return eamodule;
}
