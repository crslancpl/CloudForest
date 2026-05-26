#ifndef PYTHON_TOOL_H_
#define PYTHON_TOOL_H_

#include "datatypes/common.h"
#include <Python.h>
#include <pytypedefs.h>
#include <string>

void RunCallback(PyObject* callbacklist, PyObject* args);
void CheckList(PyObject* list);
void AddToList(PyObject* list, PyObject* obj);
void RemoveFromList(PyObject* list, PyObject* obj);

void Execute(const std::string &code);
void ExecuteFile(const std::string &path);

PyThreadState* GetMainThreadState();

void ReleaseThreadLock();
void RestoreThreadLock();
void PrintGILState();

PyObject* GetPyDictFromZRange(const ZRange &range);

#endif
