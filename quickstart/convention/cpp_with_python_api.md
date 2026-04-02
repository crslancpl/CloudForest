# C++ with python C API
## File Naming
All soruce codes about python C API are placed under `pythonbackend` directory. Source files for Python module/class definition and other Python related functions will have lower snake case with `_Py` suffix.
```
file_name_Py.h
file_name_Py.cpp
```

For module definition files, add `mod_Py` suffix.
```
module_name_mod_Py.h
module_name_mod_Py.cpp
```

For class definition files, add `class_Py` suffix.
```
class_name_class_Py.h
class_name_class_Py.cpp
```
## Source Code Naming
Initializer should be named `PyInit_<module name>_module`
```cpp
PyMODINIT_FUNC PyInit_module_name_module();
```

Name the functions in module files with snake case.
```cpp
void module_name_py_function();
```
