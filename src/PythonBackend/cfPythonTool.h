#ifndef CFPYTHONTOOL_H_
#define CFPYTHONTOOL_H_

#include <Python.h>
#include "../Global.h"

const result::GetText* GetLspMessage(request::PyGetLspMessage* req);
void InitPythonTool();
#endif
