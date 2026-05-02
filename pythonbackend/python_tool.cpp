#include "python_tool.h"

#include <Python.h>
#include <object.h>

void RunCallback(PyObject* callbacklist, PyObject* args){
    // callbacks should always be a list
    for(size_t itr = 0; itr < PyList_GET_SIZE(callbacklist); itr++){
        PyObject* callback = PyList_GET_ITEM(callbacklist, itr);
        // sometimes the value is None because we use
        // PyList_SET_ITEM(list, itr, Py_None) to remove the callback
        if(PyCallable_Check(callback)){
            PyObject_CallObject(callback, args);
        }
    }
}
