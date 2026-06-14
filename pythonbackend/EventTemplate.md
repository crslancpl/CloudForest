# Template for PythonEvent
## Create map
```cpp
static PythonEventMap event_map = {
    {EVENT___, PythonEvent()}
}
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
static PyObject* ___add_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    auto itr = event_map.find(event);
    if (itr != event_map.end()){
        itr->second.Connect(callback);
    }

    Py_RETURN_NONE;
}

// remove
static PyObject* ___rm_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    auto itr = event_map.find(event);
    if (itr != event_map.end()){
        itr->second.Disconnect(callback);
    }

    Py_RETURN_NONE;
}
```
