#include "language_mod_Py.h"

#include "pythonbackend/language/language_class_Py.h"
#include "pythonbackend/python_if.h"
#include "src/AppContext.h"
#include "src/languages/LanguageManager.h"
#include "src/gui/editarea/EditArea.h"
#include "src/languages/Language.h"
#include "src/languages/LanguageManager.h"
#include "toolset/event/Event.h"

#include <abstract.h>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>
#include <longobject.h>
#include <memory>
#include <methodobject.h>
#include <modsupport.h>
#include <object.h>
#include <pythonrun.h>
#include <pytypedefs.h>
#include <setobject.h>
#include <tupleobject.h>
#include <unicodeobject.h>


static void on_new_language_added(Language* lang){
    //
}

static PyObject *language_module_clear_data(PyObject *self, PyObject *args){
    Py_RETURN_NONE;
}

static PyObject *language_module_add(PyObject *self, PyObject *args){
    char* langname;
    char* id;
    char* syntaxfile;
    PyObject* fileextlist; //list[str]

    if(!PyArg_ParseTuple(args, "sssO", &langname, &id, &syntaxfile, &fileextlist)){
        Py_RETURN_NAN;
    }

    std::unique_ptr<LanguageInfo> info = std::make_unique<LanguageInfo>();// Free by lang manager
    info->name = strdup(langname);
    info->id = strdup(id);
    info->syntaxTemplateFile = syntaxfile;

    int extcount = PyList_GET_SIZE(fileextlist);

    for (int i = 0; i < extcount; i++) {
        info->fileExtensions.emplace(
            PyUnicode_AsUTF8(PyList_GetItem(fileextlist, i))
        );
    }

    Language& lang = pybackend::GetAppContext().langManager->CreateLanguage(std::move(info));

    Py_RETURN_NONE;
}

static PyObject *language_module_find(PyObject *self, PyObject *args){
    char* langname;

    if(!PyArg_ParseTuple(args, "s", &langname)){
        Py_RETURN_NAN;
    }

    Language* lang = pybackend::GetAppContext().langManager->FindByName(langname);

    if (lang) {
        return (PyObject*)lang->GetPyLanguage();
    }

    Py_RETURN_NONE;
}


static PyMethodDef language_module_method[] = {
    {"clear_data",  language_module_clear_data, METH_VARARGS,"clear language datas. call this before reloading the language panel"},
    {"add",  language_module_add, METH_VARARGS,"add a language to the language list."},
    {"find", language_module_find, METH_VARARGS, "find a language by name"},
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
    PyModule_AddObject(langmodule, "Language", (PyObject*)PyInit_py_Language_class());

    pybackend::GetAppContext().langManager->ENewLanguage.Connect(on_new_language_added);

    return langmodule;
}
