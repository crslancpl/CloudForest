#include "setting_mod_Py.h"

#include "src/Setting.h"

#include <object.h>
#include <pytypedefs.h>

static PyObject* setting_module_add_enabled_extension(PyObject* self, PyObject* args){
    char* name;
    char* folderPath;
    char* description;

    if(!PyArg_ParseTuple(args, "sss", &name, &folderPath, &description, &description)){
        Py_RETURN_NAN;
    }

    setting::AddExtension(name, folderPath, description, true);

    Py_RETURN_NONE;
}

static PyMethodDef setting_moddule_method[] = {
    {"add_enabled_extension",  setting_module_add_enabled_extension, METH_VARARGS,"add an enabled extension"},
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
    PyObject *settingmodule = PyModule_Create(&setting_moddule);
    return settingmodule;
}
