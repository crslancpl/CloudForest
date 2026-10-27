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
#include <sys/stat.h>
#include <unicodeobject.h>

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

static bool is_allow_restore_lock = true;
static bool is_extension_call_paused = false;
static PyThreadState* thread_state;

void ReleaseThreadLock(){
    //printf("> GIL release thread\n");
    thread_state = PyEval_SaveThread();
}

bool TryRestoreThreadLock(){
    //printf("> GIL restore thread\n");
    if (!is_allow_restore_lock) {
        return false;
    }

    if(!PyGILState_Check()){
        PyEval_RestoreThread(thread_state);
    }
    return true;
}

void BlockRestoringThreadLock(){
    //printf("block thread lock\n");
    is_allow_restore_lock = false;
}

void AllowRestoringThreadLock(){
    //printf("allow thread lock\n");
    is_allow_restore_lock = true;
}

void PauseExtensionCall(){
    //printf("pause extension call\n");
    is_extension_call_paused = true;
}

void ResumeExtensionCall(){
    //printf("resume extension call\n");
    is_extension_call_paused = false;
}

bool GetIsExtensionPausedCall(){
    return is_extension_call_paused;
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
