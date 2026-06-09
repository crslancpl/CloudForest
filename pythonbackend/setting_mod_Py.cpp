#include "setting_mod_Py.h"

#include "datatypes/common.h"
#include "datatypes/extension.h"
#include "pythonbackend/python_tool.h"
#include "src/Setting.h"

#include <cstring>
#include <listobject.h>
#include <object.h>
#include <pytypedefs.h>
#include <tupleobject.h>
#include <unicodeobject.h>

static PyObject* extension_enabled_callback_list;
static PyObject* extension_disabled_callback_list;

static void OnExtensionEnabled(Extension* extension){
    PyObject* arg = PyTuple_Pack(1, PyUnicode_FromString(extension->folder));
    RunCallback(extension_enabled_callback_list, arg);
    Py_DECREF(arg);
}

static void OnExtensionDisabled(Extension* extension){
    PyObject* arg = PyTuple_Pack(1, PyUnicode_FromString(extension->folder));
    RunCallback(extension_disabled_callback_list, arg);
    Py_DECREF(arg);
}

static PyObject* setting_module_add_extension(PyObject* self, PyObject* args){
    char* name;
    char* folder;
    char* description;
    int enable;// be careful this has to be int not bool

    if(!PyArg_ParseTuple(args, "sssp", &name, &folder, &description, &enable)){
        Py_RETURN_NAN;
    }

    Extension* extension = new Extension();// freed on "setting::RemoveExtension()" or app closed
    extension->name = strdup(name);
    extension->folder = strdup(folder);
    extension->description = strdup(description);
    extension->enabled = enable;

    setting::AddExtension(extension);

    Py_RETURN_NONE;
}

static PyObject* setting_module_add_callback(PyObject* self, PyObject* args){
    char* event;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    if (strcmp(event, "extension-enabled") == 0) {
        PyList_Append(extension_enabled_callback_list, callback);
    }else if (strcmp(event, "extension-disabled") == 0) {
        PyList_Append(extension_disabled_callback_list, callback);
    }

    Py_RETURN_NONE;
}

static PyMethodDef setting_moddule_method[] = {
    {"add_extension",  setting_module_add_extension, METH_VARARGS,"add an enabled extension"},
    {"add_callback",  setting_module_add_callback, METH_VARARGS,"add callback"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef setting_moddule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "setting",
    .m_size = 0,
    .m_methods = setting_moddule_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_setting_module(){
    extension_enabled_callback_list = PyList_New(0);
    extension_disabled_callback_list = PyList_New(0);
    setting::ListenEvent(setting::SETTING_EXTENSION_ENABLED, (EventCallback)OnExtensionEnabled);
    setting::ListenEvent(setting::SETTING_EXTENSION_DISABLED, (EventCallback)OnExtensionDisabled);
    PyObject *settingmodule = PyModule_Create(&setting_moddule);
    return settingmodule;
}
