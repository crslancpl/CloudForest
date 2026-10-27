#include "language_class_Py.h"

#include "pythonbackend/python_tool.h"
#include "src/languages/Language.h"

#include <cstdio>
#include <floatobject.h>
#include <iterobject.h>
#include <memory>
#include <modsupport.h>
#include <object.h>
#include <pytypedefs.h>
#include <tupleobject.h>
#include <unicodeobject.h>
#include <vector>

#define EVENT_NEW_EDITAREA "new-editarea"
#define EVENT_FIRST_USED "first-used"


/*
 * Callbacks
 */

static void on_new_edit_area(const Language::EditAreaAddedArgs& args){
    py_Language* py_lang = args.lang->GetPyLanguage();

    PythonEvent* event = py_lang->events->Find(EVENT_NEW_EDITAREA);

    if (!event) {
        return;
    }

    if (!TryRestoreThreadLock()) {
        return;
    }

    py_EditArea *py_ea = args.ea->GetPyEditArea();

    if (py_ea) {
        PyObject* args = PyTuple_Pack(1, py_ea);
        event->Invoke(args);
        Py_DECREF(args);
    }

    ReleaseThreadLock();
}

static void on_first_used(Language* lang){
    py_Language* py_lang = lang->GetPyLanguage();

    PythonEvent* event = py_lang->events->Find(EVENT_FIRST_USED);

    if (event) {
        if (!TryRestoreThreadLock()) {
            return;
        }
        PyObject* args = PyTuple_Pack(1, py_lang);
        event->Invoke(args);
        Py_DECREF(args);

        ReleaseThreadLock();
    }
}

/*
 * Class methods
 */

static PyObject* py_Language_get_name(py_Language* self, PyObject* args){
    const char* name = self->lang->GetInfo()->name.c_str();
    return PyUnicode_FromString(name);
}

static PyObject* py_Language_get_id(py_Language* self, PyObject* args){
    const char* id = self->lang->GetInfo()->id.c_str();
    return PyUnicode_FromString(id);
}


static PyObject* py_Language_connect(py_Language* self, PyObject* args){
    char* signal;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "sO", &signal, &callback)) {
        Py_RETURN_NAN;
    }

    if (!PyCallable_Check(callback)) {
        Py_RETURN_NAN;
    }

    PythonEvent* event = self->events->Find(signal);
    if (event) {
        event->Connect(callback);
    }

    Py_RETURN_NONE;
}

static PyObject* py_Language_disconnect(py_Language* self, PyObject* args){
    char* signal;
    PyObject* callback;
    if (!PyArg_ParseTuple(args, "sO", &signal, callback)) {
        Py_RETURN_NAN;
    }
    if (!PyCallIter_Check(callback)) {
        Py_RETURN_NAN;
    }

    PythonEvent* event = self->events->Find(signal);
    if (event) {
        event->Disconnect(callback);
    }

    Py_RETURN_NONE;
}


/*
 * Class api
 */

static PyMethodDef py_Language_class_method[]={
    {"get_name", (PyCFunction)py_Language_get_name, METH_VARARGS, "get name"},
    {"get_id", (PyCFunction)py_Language_get_id, METH_VARARGS, "get id"},
    {"connect", (PyCFunction)py_Language_connect, METH_VARARGS, "connect a function to signal"},
    {"disconnect", (PyCFunction)py_Language_disconnect, METH_VARARGS, "disconnect a function from signal"},
    {NULL, NULL, 0, NULL}
};

static PyObject* py_Language_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    py_Language *self = (py_Language*)type->tp_alloc(type, 0);

    static const std::vector<std::string> signals = {
        EVENT_FIRST_USED,
        EVENT_NEW_EDITAREA
    };

    self->events = std::make_unique<PythonEventContainer>(signals);

    return (PyObject*) self;
}

static void py_Language_dealloc(PyObject* self){
    py_Language* py_lan = (py_Language*) self;
}


static PyTypeObject py_Language_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "Language",
    .tp_basicsize = sizeof(py_Language),
    .tp_dealloc = py_Language_dealloc,
    .tp_doc = "Language of the CloudForest",
    .tp_methods = py_Language_class_method,
    .tp_new = py_Language_new,
};


/*
 * Type initializer
 */

PyTypeObject* PyInit_py_Language_class(){
    if(PyType_Ready(&py_Language_class) < 0){
        printf("language_class_Py: cannot build language python type\n");
        return nullptr;
    }
    return &py_Language_class;
}

/*
 * Public function
 */

py_Language* py_Language_create_object(Language* lang){
    lang->EFirstUsed.Connect(on_first_used);
    lang->EEditAreaAdded.Connect(on_new_edit_area);

    py_Language* py_lang = (py_Language*)PyObject_CallObject((PyObject*)&py_Language_class, nullptr);

    py_lang->lang = lang;
    return py_lang;
}
