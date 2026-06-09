#ifndef EXTENSION_DATA_H_
#define EXTENSION_DATA_H_

#include <Python.h>

typedef struct Extension {
    const char* name;
    const char* folder;
    const char* description;
    bool enabled;
    PyObject* module;
} Extension;

#endif
