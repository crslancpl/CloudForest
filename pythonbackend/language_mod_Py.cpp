#include "language_mod_Py.h"

#include "src/languages/LanguageManager_if.h"

#include <object.h>
#include <pytypedefs.h>

static PyObject *language_module_clear_data(PyObject *self, PyObject *args){
    Py_RETURN_NONE;
}

static PyObject *language_module_add_language(PyObject *self, PyObject *args){
    char* langname;
    char* id;
    char* syntaxfile;
    char* fileextension;
    if(!PyArg_ParseTuple(args, "ssss", &langname, &id, &syntaxfile, &fileextension)){
        return nullptr;
    }

    langmanager::NewLanguage(langname, id, syntaxfile, fileextension);
    Py_RETURN_NONE;
}

static PyObject *language_module_add_callback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callback;
    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        return nullptr;
    }

    if(strcmp(event, "new-editarea") == 0){
        //l
    }

    Py_RETURN_NONE;
}

static PyObject *language_module_listen_language(PyObject *self, PyObject *args){
    char* lang;
    PyObject* callback;
    if(!PyArg_ParseTuple(args, "sO", &lang, &callback)){
        return nullptr;
    }

    if(strcmp(lang, "new-editarea") == 0){
        //l
    }

    Py_RETURN_NONE;
}


static PyMethodDef language_module_method[] = {
    {"clear_data",  language_module_clear_data, METH_VARARGS,"clear language datas. call this before reloading the language panel"},
    {"add_language",  language_module_add_language, METH_VARARGS,"add a language to the language list."},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef language_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "language",
    .m_size = 0,
    .m_methods = language_module_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_language_module(){
    PyObject *langmodule = PyModule_Create(&language_module);
    return langmodule;
}
