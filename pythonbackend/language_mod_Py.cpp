#include "language_mod_Py.h"

#include "datatypes/common.h"
#include "src/gui/editarea/EditArea.h"
#include "src/languages/LanguageListener.h"
#include "src/languages/LanguageManager_if.h"
#include "python_tool.h"
#include "editarea/editarea_mod_Py.h"
#include "toolset/tools/Tool.h"

#include <abstract.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>

#include <methodobject.h>
#include <modsupport.h>
#include <object.h>
#include <pytypedefs.h>
#include <setobject.h>
#include <tupleobject.h>
#include <unicodeobject.h>

static PyObject *lang_to_callbacks_dict;
static PyObject *lang_used_callbacks_dict;

void language_module_invoke_language_used(const char* langname){
    RestoreThreadLock();
    PyObject* callbacklist = PyDict_GetItemString(lang_used_callbacks_dict, langname);
    if(callbacklist == NULL){
        return;
    }
    PyObject* arg = PyTuple_Pack(1, PyUnicode_FromString(langname));
    RunCallback(callbacklist, arg);
    ReleaseThreadLock();
}

void language_module_invoke_new_editarea(const char* langname, EditArea* ea){
    RestoreThreadLock();
    PyObject* callbacklist = PyDict_GetItemString(lang_to_callbacks_dict, langname);
    if(callbacklist == NULL){
        return;
    }
    PyObject* arg = PyTuple_Pack(1, find_editarea_py(ea));
    RunCallback(callbacklist, arg);
    ReleaseThreadLock();
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
        Py_RETURN_NAN;
    }

    Language* lang = new Language();// Free by lang manager
    lang->name = strdup(langname);
    lang->id = strdup(id);
    lang->syntaxTemplateFile = syntaxfile;

    auto extlist = tools::TrimText(fileextension, "[],' ");
    for(auto ext : extlist){
        lang->fileExtensions.emplace(ext);
    }

    langmanager::NewLanguage(lang);

    Py_RETURN_NONE;
}

static PyObject *language_module_listen_language_used(PyObject *self, PyObject* args){
    char* langname;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PyObject* callbacklist = PyDict_GetItemString(lang_used_callbacks_dict, langname);

    if(callbacklist == nullptr){
        callbacklist = PyList_New(0);
        PyDict_SetItemString(lang_used_callbacks_dict, langname, callbacklist);
        ListenNewEditAreaForLanguage(langname, language_module_invoke_new_editarea);
    }

    AddToList(callbacklist, callback);

    Py_RETURN_NONE;
}

static PyObject *language_module_get_all_editareas(PyObject *self, PyObject *args){
    PyObject* editarealist = PyList_New(0);
    char* langname;

    if(!PyArg_ParseTuple(args, "s", &langname)){
        Py_RETURN_NAN;
    }

    Language* lang = langmanager::FindLanguage(langname);
    auto set = langmanager::GetEditAreasFromLanguage(lang);
    for (const EditArea* ea : set){
        PyList_Append(editarealist, (PyObject*) find_editarea_py(ea));
    }

    return editarealist;
}


static PyObject *language_module_listen_for_editarea(PyObject *self, PyObject *args){
    char* langname;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }
    Py_RETURN_NONE;
    PyObject* callbacklist = PyDict_GetItemString(lang_to_callbacks_dict, langname);

    if(callbacklist == nullptr){
        callbacklist = PyList_New(0);
        PyDict_SetItemString(lang_to_callbacks_dict, langname, callbacklist);
        ListenNewEditAreaForLanguage(langname, language_module_invoke_new_editarea);
    }

    AddToList(callbacklist, callback);

    Py_RETURN_NONE;
}

static PyObject *language_module_stop_listen_for_editarea(PyObject *self, PyObject *args){
    char* langname;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PyObject* callbacklist = PyDict_GetItemString(lang_to_callbacks_dict, langname);
    if(callbacklist == Py_None){
        Py_RETURN_NAN;
    }

    RemoveFromList(callbacklist, callback);

    Py_RETURN_NONE;
}


static PyMethodDef language_module_method[] = {
    {"clear_data",  language_module_clear_data, METH_VARARGS,"clear language datas. call this before reloading the language panel"},
    {"add_language",  language_module_add_language, METH_VARARGS,"add a language to the language list."},
    {"listen_language_used", language_module_listen_language_used, METH_VARARGS, "the language used for the first time"},
    {"get_all_editareas", language_module_get_all_editareas, METH_VARARGS, "get a list of EditAreas from language name "},
    {"listen_for_editarea", language_module_listen_for_editarea, METH_VARARGS, "listen for new editarea with specific language"},
    {"stop_listen_for_editarea", language_module_stop_listen_for_editarea, METH_VARARGS, "stop listen for new editarea with specific language"},
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
    lang_used_callbacks_dict = PyDict_New();
    PyObject *langmodule = PyModule_Create(&language_module);
    return langmodule;
}
