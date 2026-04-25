#ifndef LANGUAGE_MOD_PY_H_
#define LANGUAGE_MOD_PY_H_

#include <Python.h>

class EditArea;

PyMODINIT_FUNC PyInit_language_module();

void language_module_invoke_new_editarea(const char* lang, EditArea* ea);

#endif
