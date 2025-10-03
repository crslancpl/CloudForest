#ifndef CFPYTHONTOOL_H_
#define CFPYTHONTOOL_H_

#include <Python.h>
#include "../Global.h"

#include "../requests/PyRequests.h"

const result::GetText* GetLspMessage(PyGetLspMessage* req);
void InitPythonTool();
#endif
