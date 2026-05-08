#include "python_tool.h"

#include <Python.h>
#include <cstdio>
#include <listobject.h>
#include <object.h>
#include <string>

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

void CheckList(PyObject *list){
    unsigned int size = PyList_GET_SIZE(list);
    printf("list size: %u\n", size);
    for(unsigned int i = 0; i < size; i++){
        if(PyList_GetItem(list, i) == Py_None){
            printf("number %u is none\n", i);
        }
    }
}

void AddToList(PyObject* list, PyObject* obj){
    PyList_Append(list, obj);
    return;
    /*
    unsigned int size = PyList_GET_SIZE(list);
    for(unsigned int i = 0; i < size; i++){
        if(PyList_GetItem(list, i) == Py_None){

            PyList_SET_ITEM(list, i, obj);
            printf("replace %i from list\n", i);
            return;
        }
    }
    */
}

void RemoveFromList(PyObject* list, PyObject* obj){
    for (unsigned int i = 0; i < PyList_GET_SIZE(list); i++) {
        if(PyObject_RichCompare(obj, PyList_GetItem(list, i), Py_EQ) != Py_None){
            PyList_SetItem(list, i, Py_None);
        }
    }
}

void Execute(const std::string &code){
    PyRun_SimpleString(code.c_str());
}

void ExecuteFile(const std::string &path){
    FILE *f = fopen(path.c_str(),"r");
    if(!f){
        printf("file: %s not found\n", path.c_str());
        return;
    }
    PyRun_SimpleFile(f, path.c_str());
}
