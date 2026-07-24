#include "python_tool.h"
#include "datatypes/common.h"

#include <Python.h>
#include <abstract.h>
#include <ceval.h>
#include <cstdio>
#include <dictobject.h>
#include <floatobject.h>
#include <iterobject.h>
#include <listobject.h>
#include <longobject.h>
#include <object.h>
#include <pystate.h>
#include <pytypedefs.h>
#include <string>
#include <unicodeobject.h>


/*
 * PythonEvents
 */


void PythonEvent::Connect(PyObject* callback){
    if(PyCallable_Check(callback)){
        Py_INCREF(callback);
        m_callbacksSet.emplace(callback);
    }
}

void PythonEvent::Disconnect(PyObject* callback){
    if(PyCallable_Check(callback)){
        for (PyObject* cb : m_callbacksSet) {
            if(Py_Is(cb, callback)){
                Py_DECREF(callback);
                m_callbacksSet.erase(callback);
            }
        }
    }
}

void PythonEvent::Invoke(PyObject* args){
    for (PyObject* callback : m_callbacksSet) {
        if(!PyCallable_Check(callback)){
            // check again
            m_callbacksSet.erase(callback);
            continue;
        }

        PyObject_CallObject(callback, args);
    }
}

/*
 * PythonEvent is recommended
 */
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

/*
 * Threading
 */
PyThreadState* thread_state;
void ReleaseThreadLock(){
    //printf("> GIL release thread\n");
    thread_state = PyEval_SaveThread();
}

void RestoreThreadLock(){
    //printf("> GIL restore thread\n");
    if(!PyGILState_Check()){
        PyEval_RestoreThread(thread_state);
    }
}

void PrintGILState(){
    if(PyGILState_Check()){
        printf("gil is available\n");
    }else {
        printf("gil is not available\n");
    }
}

PyObject* GetPyDictFromZRange(const ZRange &range){
    PyObject* rangedict = PyDict_New();
    PyObject* startdict = PyDict_New();
    PyObject* enddict = PyDict_New();

    PyDict_SetItemString(startdict, "line", PyLong_FromLong(range.start.line));
    PyDict_SetItemString(startdict, "character", PyLong_FromLong(range.start.column));
    PyDict_SetItemString(enddict, "line", PyLong_FromLong(range.end.line));
    PyDict_SetItemString(enddict, "character", PyLong_FromLong(range.end.column));

    PyDict_SetItemString(rangedict, "start", startdict);
    PyDict_SetItemString(rangedict, "end", enddict);

    Py_DECREF(startdict);
    Py_DECREF(enddict);

    return rangedict;
}

ZRange GetZRangeFromPyDict(PyObject* dict){
    ZRange range;
    PyObject *line, *col;

    PyObject* start = PyDict_GetItemString(dict, "start");

    line = PyDict_GetItemString(start, "line");
    range.start.line = PyLong_AS_LONG(line);
    Py_DECREF(line);

    col = PyDict_GetItemString(start, "character");
    range.start.column = PyLong_AS_LONG(col);
    Py_DECREF(col);

    Py_DECREF(start);

    PyObject* end = PyDict_GetItemString(dict, "end");

    line = PyDict_GetItemString(end, "line");
    range.end.line = PyLong_AS_LONG(line);
    Py_DECREF(line);

    col = PyDict_GetItemString(end, "character");
    range.end.column = PyLong_AS_LONG(col);
    Py_DECREF(col);

    Py_DECREF(start);

    return range;
}
