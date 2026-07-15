#ifndef EDITAREA_MOD_PY_H_
#define EDITAREA_MOD_PY_H_

#include <Python.h>

class EditArea;
typedef struct py_EditArea py_EditArea;

PyMODINIT_FUNC PyInit_editarea_module();

#endif
