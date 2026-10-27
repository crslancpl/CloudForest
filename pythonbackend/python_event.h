#ifndef PYTHON_EVENT_H_
#define PYTHON_EVENT_H_

#include <Python.h>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

class PythonEvent{
public:
    PythonEvent();
    ~PythonEvent();

    void Connect(PyObject* callback);
    void Disconnect(PyObject* callback);
    void Invoke(PyObject* args);

protected:
    std::unordered_set<PyObject*> m_callbacksSet;
};

class PythonEventContainer{
public:
    PythonEventContainer();
    PythonEventContainer(const std::vector<std::string>& signals);

    void Add(const std::string& signal);

    PythonEvent* Find(const std::string& signal);

private:
    std::unordered_map<std::string, PythonEvent> m_container;
};

#endif
