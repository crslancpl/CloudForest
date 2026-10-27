#include "lsp_client_class.h"
#include "pythonbackend/python_tool.h"

#include <toolset/lspbackend/LspClient.h>

static PyObject* py_LspClient_restart(py_LspClient* self, PyObject *args){
    self->client->EndServer();
    self->client->StartServer();
    Py_RETURN_NONE;
}

static PyObject *py_EditArea_add_callback(py_LspClient *self, PyObject *args){
    char *signal;
    PyObject *callback;

    if(!PyArg_ParseTuple(args, "sO", &signal, &callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* event = self->events.Find(signal);
    if (event) {
        event->Connect(callback);
    }

    Py_RETURN_NONE;
}

static PyObject *py_EditArea_rm_callback(py_LspClient *self, PyObject *args){
    static unsigned int rm_id = 0;
    char *signal;
    PyObject *callback;

    if(!PyArg_ParseTuple(args, "sO",&signal,&callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* event = self->events.Find(signal);
    if (event) {
        event->Disconnect(callback);
    }

    Py_RETURN_NONE;
}

static PyMethodDef py_LspClient_class_method[]={
    {"restart", (PyCFunction)py_LspClient_restart, METH_VARARGS, "restart server"},
    {NULL, NULL, 0, NULL}
};

static PyObject* py_LspClient_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    py_LspClient *self = (py_LspClient*)type->tp_alloc(type, 0);

    return (PyObject*) self;
}

PyTypeObject py_LspClient_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "LspClient",
    .tp_basicsize = sizeof(py_LspClient),
    .tp_doc = "LspClient of the CloudForest",
    .tp_methods = py_LspClient_class_method,
    .tp_new = py_LspClient_new,
};

PyTypeObject* PyInit_py_LspClient_class(){
    if(PyType_Ready(&py_LspClient_class)<0){
        printf("LspClient_class_Py: cannot build edit area python type\n");
        return nullptr;
    }
    return &py_LspClient_class;
}
