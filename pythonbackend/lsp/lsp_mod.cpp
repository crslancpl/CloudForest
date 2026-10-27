#include "lsp_mod.h"

#include "../python_if.h"
#include "src/AppContext.h"
#include "src/languages/Language.h"
#include "toolset/lspbackend/LspClient.h"
#include <memory>

static PyObject *lsp_module_add(PyObject *self, PyObject *args){
    char* servercommand;
    char* langname;

    if(!PyArg_ParseTuple(args, "ss", &langname)){
        Py_RETURN_NAN;
    }

    Language* lang = pybackend::GetAppContext().langManager->FindByName(langname);

    if(lang != nullptr){
        std::unique_ptr<LspClient> cl = std::make_unique<LspClient>(servercommand, langname);
    };

    Py_RETURN_NONE;
}


static PyMethodDef lsp_module_method[] = {
    {"add",  lsp_module_add, METH_VARARGS,"add a new lsp, first param: language name, second paramm: lsp command"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef lsp_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "language",
    .m_size = 0,
    .m_methods = lsp_module_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_lsp_module(){
    PyObject *lspmodule = PyModule_Create(&lsp_module);
    return lspmodule;
}
