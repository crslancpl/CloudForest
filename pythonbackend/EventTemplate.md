# Template for PythonEvent
## Create map
```cpp
static PythonEventContainer events;
```
## Calling callback
```cpp
static void Call(){
    RestoreThreadLock();
    PyObject* args = PyTuple_Pack(0);
    PythonEvent &event = event_map.at(EVENT___);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}
```
## Add callback
```cpp
// add
static PyObject* _connect(PyObject *self, PyObject *args){
    char* signal;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &signal, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* event = events.Find(signal);
    if (event) {
        event.Connect(callback);
    }

    Py_RETURN_NONE;
}

// remove
static PyObject* _disconnect(PyObject *self, PyObject *args){
    char* signal;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &signal, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* event = events.Find(signal);
    if (event) {
        event.Disconnect(callback);
    }

    Py_RETURN_NONE;
}
```
