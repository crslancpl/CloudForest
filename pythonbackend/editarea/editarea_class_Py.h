#ifndef EDITAREA_CLASS_PY_H_
#define EDITAREA_CLASS_PY_H_

#include "pythonbackend/python_event.h"

#include <Python.h>
#include <memory>


class EditArea;
/*
 * Class
 */
typedef struct py_EditArea{
    PyObject_HEAD;
    EditArea *editarea;
    std::unique_ptr<PythonEventContainer> events;
} py_EditArea;


/*
 * type initializer
 */

PyTypeObject* PyInit_py_EditArea_class();

/*
 * public function
 */

py_EditArea* py_EditArea_create_object(EditArea* ea);


#endif
