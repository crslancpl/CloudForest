#include "editarea_mod_Py.h"

#include "editarea_class_Py.h"

#include "src/gui/editarea/EditArea.h"
#include "src/gui/editarea/EditArea_if.h"
#include "src/gui/editarea/LspPopovers_if.h"


/*
 * Module
 */

// lists
static PyObject *registered_editareas = nullptr;
static PyObject *ea_registered_callback_list = nullptr;
static PyObject *ea_language_changed_callback_list = nullptr;


static void RunCallback(PyObject* callbacklist, PyObject* args){
    for(size_t itr = 0; itr < PyList_GET_SIZE(callbacklist); itr++){
        PyObject* callback = PyList_GET_ITEM(callbacklist, itr);
        PyObject_CallObject(callback, args);
    }
}

static py_EditArea* FindEditAreaForPython(EditArea *ea){
    if (registered_editareas == nullptr) {
        return nullptr;
    }
    for(size_t itr = 0; itr < PyList_GET_SIZE(registered_editareas); itr++){
        py_EditArea *registeredea = (py_EditArea*)PyList_GET_ITEM(registered_editareas, itr);
        if(registeredea->Editarea == ea){
            return registeredea;
        }
    }
    return nullptr;
}

void editarea_py_invoke_text_changed(EditArea *ea){
    py_EditArea* py_ea = FindEditAreaForPython(ea);
    if(py_ea == nullptr) return;
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->TextchangedCallbacks, args);
    Py_DECREF((PyObject*) py_ea);
    Py_DECREF((PyObject*) args);
}

void editarea_py_invoke_cursor_moved(EditArea *ea, int line, int column){
    py_EditArea* py_ea = FindEditAreaForPython(ea);
    if(py_ea == nullptr) return;
    PyObject* args = PyTuple_Pack(3, py_ea, PyLong_FromLong(line), PyLong_FromLong(column));
    RunCallback(py_ea->CursorMovedCallbacks, args);
    Py_DECREF((PyObject*) py_ea);
    Py_DECREF((PyObject*) args);
}

void editarea_py_invoke_completion_requested(EditArea *editarea){
    py_EditArea* py_ea = FindEditAreaForPython(editarea);
    if(py_ea == nullptr) return;
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->CompletionRequestedCallbacks, args);
    Py_DECREF((PyObject*) py_ea);
    Py_DECREF((PyObject*) args);
}

void editarea_py_invoke_file_saved(EditArea *editarea){
    py_EditArea* py_ea = FindEditAreaForPython(editarea);
    if(py_ea == nullptr) return;
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->FileSavedCallbacks, args);
    Py_DECREF((PyObject*) py_ea);
    Py_DECREF((PyObject*) args);
}

void editarea_py_invoke_filedata_changed(EditArea *editarea){
    py_EditArea* py_ea = FindEditAreaForPython(editarea);
    if(py_ea == nullptr) return;
    PyObject* args = PyTuple_Pack(1, py_ea);
    RunCallback(py_ea->FileDataChangedCallbacks, args);
    Py_DECREF((PyObject*) py_ea);
    Py_DECREF((PyObject*) args);
}

static PyObject *editarea_module_addcallback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callbackfunc;
    if(!PyArg_ParseTuple(args, "sO", &event, &callbackfunc)){
        return nullptr;
    }
    if(!PyCallable_Check(callbackfunc)){
        return nullptr;
    }

    if (strcmp(event, "NEWEDITAREA") == 0) {
        PyList_Append(ea_registered_callback_list, callbackfunc);
    } else if(strcmp(event, "LANGUAGECHANGED") == 0){
        PyList_Append(ea_language_changed_callback_list, callbackfunc);
    }

    Py_RETURN_NONE;
}



static PyMethodDef editarea_module_methods[]={
    {"add_callback", editarea_module_addcallback, METH_VARARGS, "add callback for event"},
    {nullptr, nullptr, 0, nullptr}
};

static PyModuleDef editarea_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "editarea_mod",
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
    py_EditArea *newEa = py_EditArea_create_object();

    newEa->Editarea = ea;
    newEa->Filepath = strdup(ea->GetFilePath());



    // we are trying to make filepath a variable
    PyObject *filepath = PyUnicode_FromString(ea->GetFilePath());
    PyObject *args = PyTuple_Pack(1, newEa);
    PyList_Append(registered_editareas, (PyObject*)newEa);
    RunCallback(ea_registered_callback_list, args);
    Py_DECREF(filepath);
    Py_DECREF(args);
}

PyMODINIT_FUNC PyInit_editarea_module(){
    registered_editareas = PyList_New(0);
    ea_registered_callback_list = PyList_New(0);
    ea_language_changed_callback_list = PyList_New(0);

    PyObject *eamodule = PyModule_Create(&editarea_module);

    PyModule_AddObject(eamodule, "EditArea", (PyObject*)PyInit_py_EditArea_class());
    editarea::AddNewEditAreaCallback(editarea_py_register);
    return eamodule;
}
