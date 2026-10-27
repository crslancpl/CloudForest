#ifndef PYTHON_TOOL_H_
#define PYTHON_TOOL_H_

#include "datatypes/common.h"

#include <Python.h>
#include <pytypedefs.h>
#include <string>

void Execute(const std::string &code);
void ExecuteFile(const std::string &path);

PyThreadState* GetMainThreadState();

/*
 * GIL
 */
void ReleaseThreadLock();
bool TryRestoreThreadLock();// true if success
void BlockRestoringThreadLock();
void AllowRestoringThreadLock();
void PauseExtensionCall();
void ResumeExtensionCall();
bool GetIsExtensionPausedCall();
void PrintGILState();

PyObject* GetPyDictFromZRange(const ZRange &range);
ZRange GetZRangeFromPyDict(PyObject* dict);

#endif
