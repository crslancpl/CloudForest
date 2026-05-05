#ifndef CF_MODULE_H_
#define CF_MODULE_H_

#include <Python.h>

PyMODINIT_FUNC PyInit_cloudforest_module();
void cloudforest_module_invoke_app_closed();

#endif
