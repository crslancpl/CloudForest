#ifndef EDITAREA_CLASS_PY_H_
#define EDITAREA_CLASS_PY_H_

#include "pythonbackend/python_tool.h"
#include <Python.h>

#define PY_EDITAREA_EVENT_CLOSED "closed"
#define PY_EDITAREA_EVENT_COMPLETION_REQUESTED "completion-requested"
#define PY_EDITAREA_EVENT_CURSOR_MOVED "cursor-moved"
#define PY_EDITAREA_EVENT_FILE_DATA_CHANGED "file-data-changed"
#define PY_EDITAREA_EVENT_FILE_SAVED "file-saved"
#define PY_EDITAREA_EVENT_LANG_CHANGED "lang-changed"
#define PY_EDITAREA_EVENT_TEXT_CHANGED "text-changed"


class EditArea;
/*
 * Class
 */
typedef struct py_EditArea{
    PyObject_HEAD;
    char* filePath;
    EditArea *editarea;
    PythonEventMap *eventMap;
}py_EditArea;


py_EditArea* py_EditArea_create_object();
PyTypeObject* PyInit_py_EditArea_class();

#endif
