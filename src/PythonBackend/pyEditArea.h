#ifndef CFEDITAREA_H_
#define CFEDITAREA_H_

#include <Python.h>

#include "../requests/PyRequests.h"

class EditArea;

PyMODINIT_FUNC init_cf_EditArea_module();

void register_py_EditArea(PyRegisterEA *req);

void callback_py_EditArea_object(PyCallbackEA *req);

#endif
