#include "cloudforest_mod_Py.h"

#include "editarea/editarea_mod_Py.h"
#include "language_mod_Py.h"

static PyObject *cloudforest_module_test(PyObject *self, PyObject *args){
    printf("this is from CloudForest module\n");
    Py_RETURN_NONE;
}


static PyMethodDef cloudforest_module_method[] = {
    {"test",  cloudforest_module_test, METH_VARARGS,"test if module available"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef cloudforest_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cloudforest",
    .m_size = 0,
    .m_methods = cloudforest_module_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_cloudforest_module(){
    PyObject *cfmodule = PyModule_Create(&cloudforest_module);
    PyModule_AddObject(cfmodule, "editarea", (PyObject*)PyInit_editarea_module());
    PyModule_AddObject(cfmodule, "language", (PyObject*)PyInit_language_module());
    return cfmodule;
}
