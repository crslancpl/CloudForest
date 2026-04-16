#include "setting_panel_mod_Py.h"

#include "src/gui/settingpanel/SettingPanel_if.h"

static PyObject *setting_panel_module_add_tab(PyObject *self, PyObject *args){
    char* tabname;
    if(!PyArg_ParseTuple(args, "s", &tabname)){
        return nullptr;
    }

    settingpanel::AddSetting(tabname);
    Py_RETURN_NONE;
}


static PyMethodDef setting_panel_module_method[] = {
    {"add_tab",  setting_panel_module_add_tab, METH_VARARGS,"add a tab"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef setting_panel_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "setting_panel",
    .m_size = 0,
    .m_methods = setting_panel_module_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_setting_panel_module(){
    PyObject *settingpanelmodule = PyModule_Create(&setting_panel_module);
    return settingpanelmodule;
}
