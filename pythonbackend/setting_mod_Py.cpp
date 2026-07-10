#include "setting_mod_Py.h"

#include "datatypes/extension.h"
#include "pythonbackend/python_tool.h"
#include "src/Setting.h"

#include <listobject.h>
#include <memory.h>
#include <memory>
#include <object.h>
#include <pytypedefs.h>
#include <tupleobject.h>
#include <unicodeobject.h>
#include <unordered_map>

#define EVENT_EXTENSION_ENABLED "extension-enabled"
#define EVENT_EXTENSION_DISABLED "extension-disabled"

static PythonEventMap event_map = {
    {EVENT_EXTENSION_DISABLED, PythonEvent()},
    {EVENT_EXTENSION_ENABLED, PythonEvent()}
};

static void OnExtensionEnabled(Extension* extension){
    RestoreThreadLock();
    PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(strdup(extension->folder)));
    PythonEvent &event = event_map.at(EVENT_EXTENSION_ENABLED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static void OnExtensionDisabled(Extension* extension){
    RestoreThreadLock();
    PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(extension->folder));
    PythonEvent &event = event_map.at(EVENT_EXTENSION_DISABLED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static PyObject* setting_module_add_extension(PyObject* self, PyObject* args){
    char* name;
    char* folder;
    char* description;
    int enable;// be careful this has to be int not bool

    if(!PyArg_ParseTuple(args, "sssp", &name, &folder, &description, &enable)){
        Py_RETURN_NAN;
    }
    std::unique_ptr<Extension> extension = std::make_unique<Extension>();

    extension->name = strdup(name);
    extension->folder = strdup(folder);
    extension->description = strdup(description);
    extension->enabled = enable;

    setting::AddExtension(std::move(extension));

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

    auto itr = event_map.find(event);
    if (itr != event_map.end()){
        itr->second.Connect(callback);
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
    setting::Listen(setting::SETTING_EXTENSION_ENABLED, (EventCallback)OnExtensionEnabled);
    setting::Listen(setting::SETTING_EXTENSION_DISABLED, (EventCallback)OnExtensionDisabled);
    PyObject *settingmodule = PyModule_Create(&setting_moddule);
    return settingmodule;
}
