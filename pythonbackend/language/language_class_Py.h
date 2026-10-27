#ifndef LANGUAGE_CLASS_H_
#define LANGUAGE_CLASS_H_

#include "pythonbackend/python_event.h"

#include <Python.h>
#include <memory>

class Language;

/*
 * class
 */

typedef struct py_Language {
    PyObject_HEAD;
    Language* lang;
    std::unique_ptr<PythonEventContainer> events;
} py_Language;


/*
 * Type initializer
 */

PyTypeObject* PyInit_py_Language_class();

/*
 * Public function
 */

py_Language* py_Language_create_object(Language* lang);

#endif
