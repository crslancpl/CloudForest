#include "python_event.h"

/*
 * PythonEvents
 */

PythonEvent::PythonEvent(){}
PythonEvent::~PythonEvent(){}

void PythonEvent::Connect(PyObject* callback){
    if(PyCallable_Check(callback)){
        Py_INCREF(callback);
        m_callbacksSet.emplace(callback);
    }
}

void PythonEvent::Disconnect(PyObject* callback){
    if(PyCallable_Check(callback)){
        for (PyObject* cb : m_callbacksSet) {
            if(Py_Is(cb, callback)){
                Py_DECREF(callback);
                m_callbacksSet.erase(callback);
            }
        }
    }
}

void PythonEvent::Invoke(PyObject* args){
    for (PyObject* callback : m_callbacksSet) {
        if(!PyCallable_Check(callback)){
            // check again
            m_callbacksSet.erase(callback);
            continue;
        }

        PyObject_CallObject(callback, args);
    }
}


PythonEventContainer::PythonEventContainer(){
    //
}

PythonEventContainer::PythonEventContainer(const std::vector<std::string>& signals){
    m_container.reserve(signals.size());
    for (const std::string& signal: signals) {
        m_container.emplace(signal, PythonEvent());
    }
}

void PythonEventContainer::Add(const std::string& signal){
    m_container.emplace(signal, PythonEvent());
}

PythonEvent* PythonEventContainer::Find(const std::string& signal){
    auto itr = m_container.find(signal);
    if (itr != m_container.end()) {
        return &itr->second;
    }

    return nullptr;
}
