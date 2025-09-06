#include <Python.h>
#include <cstdio>
#include <methodobject.h>
#include <object.h>
#include "cfModule.h"
#include "../Core.h"

static PyObject *error;

static int cf_module_exec(PyObject *m){
    if (error != NULL) {
            PyErr_SetString(PyExc_ImportError,
                            "cannot initialize cf module more than once");
            return -1;
        }
        error = PyErr_NewException("cf.error", NULL, NULL);
        if (PyModule_AddObjectRef(m, "cfError", error) < 0) {
            return -1;
        }
    return 0;
}


static PyObject *cf_Test(PyObject *self, PyObject *args){
    printf("tested module\n");
    Py_RETURN_NONE;
}

static PyObject *cf_GetAllOpenedFiles(PyObject *self, PyObject *args){
    /*
    vector<shared_ptr<EditArea>> &AllEditArea = core::GetAllEditArea();
    PyObject *list = PyList_New(AllEditArea.size());
    for(shared_ptr<EditArea> &ea: AllEditArea){
        PyObject *editareafilepath = PyUnicode_FromString(ea->AbsoPath.c_str());
        PyList_Append(list, editareafilepath);
    }

    return list;
    */
    Py_RETURN_NONE;
}


static GUIAction guiaction;
static void callback(char*content){
    g_print("a\n");
}
//async
static PyObject *cf_EditArea_GetContent(PyObject *self, PyObject *args){
    char* absolutepath;
    if(!PyArg_ParseTuple(args, "s", &absolutepath)){
        return nullptr;
    }

    guiaction.Destination = Parts::GUI;
    guiaction.callback = callback;
    guiaction.actiontype = GUIAction::GETEDITAREACONTENT;
    guiaction.filename = absolutepath;
    core::Interact(&guiaction);
    g_print("b\n");
    /*
    shared_ptr<EditArea> *ea = core::GetEditAreaFromFileAbsoPath(absolutepath);
    if(ea == nullptr){
        return nullptr;
    }

    string text = (*ea)->GetContent();

    if(text.empty()){
        Py_RETURN_NONE;
    }else{
        return PyUnicode_FromString(text.c_str());
    }
    */
    Py_RETURN_NONE;
}

static PyObject *cf_EditArea_TextChanged_AddCallBack(PyObject *self, PyObject *args){
    char *absolutepath, *pyfuncname;
    if(!PyArg_ParseTuple(args, "ss", &absolutepath, &pyfuncname)){
        return nullptr;
    }
    /*
    shared_ptr<EditArea> *ea = core::GetEditAreaFromFileAbsoPath(absolutepath);
    if(ea == nullptr){
        return PyErr_NewException("Not found", nullptr, nullptr);
    }

    (*ea)->TextChangedPyCallback.emplace_back(pyfuncname);
    */
    Py_RETURN_NONE;
}

static PyObject *cf_EditArea_TextChanged_RemoveCallBack(PyObject *self, PyObject *args){
    char *absolutepath, *pyfuncname;
    if(!PyArg_ParseTuple(args, "ss", &absolutepath, &pyfuncname)){
        return nullptr;
    }
    /*
    shared_ptr<EditArea> *ea = core::GetEditAreaFromFileAbsoPath(absolutepath);
    if(ea == nullptr){
        return PyErr_NewException("Not found", nullptr, nullptr);
    }

    vector<string> &callbacks = (*ea)->TextChangedPyCallback;

    int pos = -1;
    for(string& callback : callbacks){
        pos ++;
        if(callback == pyfuncname){
            callbacks.erase(callbacks.begin() + pos);
        }
    }
    */
    Py_RETURN_NONE;
}


static PyObject *cf_EditArea_HighLight(PyObject *self, PyObject *args){
    char *absolutepath, *tagname;
    int line, offset, length;
    if(!PyArg_ParseTuple(args, "siiis", &absolutepath,&line,&offset,&length,&tagname)){
        return nullptr;
    }
    printf("highlights\n");
    /*
    EditArea *ea = core::GetEditAreaFromFileAbsoPath(absolutepath)->get();
    if(ea == nullptr){
        return nullptr;
    }
    ea->ApplyTagByLine(line, offset,length, tagname);
    */
    Py_RETURN_NONE;
}

static PyMethodDef cf_EditArea_method[]={
    {"test", cf_Test, METH_VARARGS, ""},
    {"getcontent", cf_EditArea_GetContent, METH_VARARGS, "get content from edit area"},
    {"textchanged_addcallback", cf_EditArea_TextChanged_AddCallBack, METH_VARARGS, "add callback"},
    {"textchanged_rmcallback", cf_EditArea_TextChanged_RemoveCallBack, METH_VARARGS, "remove callback"},
    {"highlight", cf_EditArea_HighLight, METH_VARARGS, "highlight line(>= 1) pos(>= 0) length(>= 0) with tagname"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef editareamodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "EditArea",
    .m_size = 0,
    .m_methods = cf_EditArea_method,
    .m_slots = nullptr
};

static PyMethodDef cf_method[] ={
    {"test",  cf_Test, METH_VARARGS,"test if module available"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef cloudforestmodule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "CloudForest",
    .m_size = 0,
    .m_methods = cf_method,
    .m_slots = nullptr
};


PyMODINIT_FUNC initcloudforestmodule(void){
    PyObject *cfmodule = PyModule_Create(&cloudforestmodule);
    PyObject *eamodule = PyModule_Create(&editareamodule);
    PyModule_AddObject(cfmodule, "EditArea", eamodule);
    return cfmodule;
}
