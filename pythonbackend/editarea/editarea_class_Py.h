#ifndef EDITAREA_CLASS_PY_H_
#define EDITAREA_CLASS_PY_H_

#include <Python.h>

class EditArea;
/*
 * Class
 */
typedef struct py_EditArea{
    PyObject_HEAD;
    char* Filepath;
    char* Lang;
    EditArea *Editarea;
    PyObject *CallbackDictionary = NULL;
    PyObject *CursorMovedCallbacks = NULL;
    PyObject *TextchangedCallbacks;
    PyObject *CompletionRequestedCallbacks;
    PyObject *LangChangedCallbacks;
    PyObject *FileSavedCallbacks;
    PyObject *FileDataChangedCallbacks;
}py_EditArea;


py_EditArea* py_EditArea_create_object();
PyTypeObject* PyInit_py_EditArea_class();

#endif
