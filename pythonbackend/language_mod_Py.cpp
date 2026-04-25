#include "language_mod_Py.h"

#include "src/languages/LanguageListener.h"
#include "src/languages/LanguageManager_if.h"
#include "python_tool.h"
#include "editarea/editarea_mod_Py.h"

#include <listobject.h>
#include <methodobject.h>
#include <object.h>
#include <pytypedefs.h>
#include <setobject.h>
#include <tupleobject.h>

static PyObject *lang_to_callbacks_dict;

void language_module_invoke_new_editarea(const char* langname, EditArea* ea){
    PyObject* callbacklist = PyDict_GetItemString(lang_to_callbacks_dict, langname);

    if(callbacklist == NULL){
        return;
    }
    PyObject* arg = PyTuple_Pack(1, find_editarea_py(ea));
    RunCallback(callbacklist, arg);
}


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

static PyObject *language_module_listen(PyObject *self, PyObject *args){
    char* langname;
    PyObject* callback;
    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        return nullptr;
    }
    if(!PyCallable_Check(callback)){
        return nullptr;
    }

    PyObject* callbacklist = PyDict_GetItemString(lang_to_callbacks_dict, langname);
    if(callbacklist == NULL){
        //create new language
        callbacklist = PyList_New(0);
        PyDict_SetItemString(lang_to_callbacks_dict, langname, callbacklist);
        ListenNewEditAreaForLanguage(langname, language_module_invoke_new_editarea);
    }

    PyList_Append(callbacklist, callback);
    Py_RETURN_NONE;
}

static PyObject *language_module_stop_listen(PyObject *self, PyObject *args){
    char* langname;
    PyObject* callback;
    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        return nullptr;
    }
    if(!PyCallable_Check(callback)){
        return nullptr;
    }

    PyObject* callbacklist = PyDict_GetItemString(lang_to_callbacks_dict, langname);
    if(callbacklist == Py_None){
        return nullptr;
    }

    PySet_Discard(callbacklist, callback);

    Py_RETURN_NONE;
}


static PyMethodDef language_module_method[] = {
    {"clear_data",  language_module_clear_data, METH_VARARGS,"clear language datas. call this before reloading the language panel"},
    {"add_language",  language_module_add_language, METH_VARARGS,"add a language to the language list."},
    {"listen", language_module_listen, METH_VARARGS, "listen for new editarea with specific language"},
    {"stop_listen", language_module_stop_listen, METH_VARARGS, "stop listen for new editarea with specific language"},
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
    lang_to_callbacks_dict = PyDict_New();
    PyObject *langmodule = PyModule_Create(&language_module);
    return langmodule;
}
