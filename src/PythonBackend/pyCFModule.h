#ifndef MODULE_H_
#define MODULE_H_

#include <Python.h>
#include "../requests/PyRequests.h"

PyMODINIT_FUNC init_cloudforest_module();
void cfmod_RunCallback(PyRunCallBack* req);
void cfmod_OpenLanguageServer(PyOpenLanguageServer* req);

#endif
