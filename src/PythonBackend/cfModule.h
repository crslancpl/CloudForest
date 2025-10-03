#ifndef MODULE_H_
#define MODULE_H_

#include <Python.h>
#include "../requests/PyRequests.h"

PyMODINIT_FUNC initcloudforestmodule();
void RunCallback(PyRunCallBack* req);

#endif
