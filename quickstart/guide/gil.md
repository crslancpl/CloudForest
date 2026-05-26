# GIL locking
> In *pythonbackend/python_if.cpp*

To prevent GIL from blocking the Python threads, GIL is released after starting the interpreter. C++ code should not held the GIL unless it is calling Python C API. Use `ReleaseThreadLock()` to release the lock and `RestoreThreadLock()` to restore the lock. Do not call `PyEval_SaveThread()` and `PyEval_RestoreThread()` manually.

## Thread Lock
Need to restore when:
* C++ code is using Python C API call and is not called by Python code

Doesn't need to restore when:
* C++ functions is called by Python code
```
(cpp -> py)
start the interpreter (GIL held automatically)
> release

(cpp -> py)
> restore
C API function call
> release

(cpp <- py)

(cpp -> py)
> restore
C API function call
> release

(cpp -> py)
> restore
end the interpreter
```

## Example of functions that require thread lock 
```cpp
void FunctionUsingPythonApi(long num){
    // restore the thread first
    RestoreThreadLock();
    
    /*
     * do something that uses Python C API functions
     */
    PyObject* obj = PyLong_FromLong(num);
    
    // release it so other threads won't be blocked
    ReleaseThreadLock();
}
```
