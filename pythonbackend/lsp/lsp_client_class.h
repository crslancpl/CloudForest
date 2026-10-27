#ifndef LSP_CLIENT_CLASS_H_
#define LSP_CLIENT_CLASS_H_

#include "pythonbackend/python_tool.h"

#include <Python.h>


class LspClient;
/*
 * Class
 */
typedef struct py_LspClient{
    PyObject_HEAD;
    LspClient *client;
    PythonEventContainer events;
}py_LspClient;

PyTypeObject* PyInit_py_LspClient_class();

#endif
