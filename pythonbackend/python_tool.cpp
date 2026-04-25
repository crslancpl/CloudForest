#include "python_tool.h"

#include <Python.h>

void RunCallback(PyObject* callbacklist, PyObject* args){
    // callbacks should always be a list
    for(size_t itr = 0; itr < PyList_GET_SIZE(callbacklist); itr++){
        PyObject* callback = PyList_GET_ITEM(callbacklist, itr);
        PyObject_CallObject(callback, args);
    }
}
