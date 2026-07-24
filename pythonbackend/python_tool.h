#ifndef PYTHON_TOOL_H_
#define PYTHON_TOOL_H_

#include "datatypes/common.h"

#include <Python.h>
#include <memory>
#include <pytypedefs.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

class PythonEvent;

typedef std::unordered_map<std::string, PythonEvent> PythonEventMap;
typedef std::unordered_map<std::string, std::unique_ptr<PythonEvent>> PythonEventPtrMap;

class PythonEvent{
public:
    virtual void Connect(PyObject* callback);
    virtual void Disconnect(PyObject* callback);
    virtual void Invoke(PyObject* args);

protected:
    std::unordered_set<PyObject*> m_callbacksSet;
};

void RunCallback(PyObject* callbacklist, PyObject* args);
void CheckList(PyObject* list);
void AddToList(PyObject* list, PyObject* obj);
void RemoveFromList(PyObject* list, PyObject* obj);

void Execute(const std::string &code);
void ExecuteFile(const std::string &path);

PyThreadState* GetMainThreadState();

/*
 * GIL
 */
void ReleaseThreadLock();
void RestoreThreadLock();
void PrintGILState();

PyObject* GetPyDictFromZRange(const ZRange &range);
ZRange GetZRangeFromPyDict(PyObject* dict);

#endif
