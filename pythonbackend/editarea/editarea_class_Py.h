#ifndef EDITAREA_CLASS_PY_H_
#define EDITAREA_CLASS_PY_H_

#include <Python.h>

class EditArea;
/*
 * Class
 */
typedef struct py_EditArea{
    PyObject_HEAD;
    char* filePath;
    char* lang;
    EditArea *editarea;
    PyObject *callbackDictionary = NULL;
    PyObject *cursorMovedCallbacks = NULL;
    PyObject *textchangedCallbacks = NULL;
    PyObject *completionRequestedCallbacks = NULL;
    PyObject *langChangedCallbacks = NULL;
    PyObject *fileSavedCallbacks = NULL;
    PyObject *fileDataChangedCallbacks = NULL;
}py_EditArea;


py_EditArea* py_EditArea_create_object();
PyTypeObject* PyInit_py_EditArea_class();

#endif
