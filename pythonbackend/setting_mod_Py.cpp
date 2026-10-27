#include "setting_mod_Py.h"

#include "datatypes/extension.h"
#include "pythonbackend/python_if.h"
#include "pythonbackend/python_tool.h"
#include "pythonbackend/python_event.h"
#include "src/session/ExtensionManager.h"
#include "src/AppContext.h"

#include <listobject.h>
#include <memory.h>
#include <memory>
#include <object.h>
#include <pytypedefs.h>
#include <tupleobject.h>
#include <unicodeobject.h>
#include <utility>

#define EVENT_EXTENSION_ENABLED "extension-enabled"
#define EVENT_EXTENSION_DISABLED "extension-disabled"

static PythonEventContainer events;

static void on_extension_enabled(Extension* extension){
    if (!TryRestoreThreadLock()) {
        return;
    }

    PythonEvent* event = events.Find(EVENT_EXTENSION_ENABLED);
    if (event) {
        PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(strdup(extension->folder)));
        event->Invoke(args);
        Py_DECREF(args);
    }

    ReleaseThreadLock();
}

static void on_extension_disabled(Extension* extension){
    if (!TryRestoreThreadLock()) {
        return;
    }

    PythonEvent* event = events.Find(EVENT_EXTENSION_DISABLED);
    if (event) {
        PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(extension->folder));
        event->Invoke(args);
        Py_DECREF(args);
    }

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

    pybackend::GetAppContext()
        .sessionManager->GetExtensionManager().AddExtension(std::move(extension));

    Py_RETURN_NONE;
}

static PyObject* setting_module_connect(PyObject* self, PyObject* args){
    char* event;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* ev = events.Find(event);
    if (ev) {
        ev->Connect(callback);
    }

    Py_RETURN_NONE;
}

static PyMethodDef setting_moddule_method[] = {
    {"add_extension",  setting_module_add_extension, METH_VARARGS,"add an enabled extension"},
    {"connect",  setting_module_connect, METH_VARARGS,"add callback"},
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
    events.Add(EVENT_EXTENSION_ENABLED);
    events.Add(EVENT_EXTENSION_DISABLED);

    ExtensionManager& manager = pybackend::GetAppContext().sessionManager->GetExtensionManager();

    manager.EExtensionEnabled.Connect(on_extension_enabled);
    manager.EExtensionDisabled.Connect(on_extension_disabled);

    PyObject *settingmodule = PyModule_Create(&setting_moddule);
    return settingmodule;
}
