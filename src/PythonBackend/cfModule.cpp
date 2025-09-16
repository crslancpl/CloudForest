#include "cfModule.h"

#include <Python.h>
#include <cstdio>
#include <cstring>
#include <methodobject.h>
#include <object.h>
#include <future>

#include "../Core.h"
#include "../Global.h"

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

static PyObject *cf_EditArea_SetLang(PyObject *self, PyObject *args){
    char* absolutepath, lang;
    if(!PyArg_ParseTuple(args, "ss", &absolutepath, lang)){
        return nullptr;
    }

    static request::EASetLang req;
    req.Filepath = absolutepath;
    req.Lang = lang;

    core::Interact(&req);

    Py_RETURN_NONE;
}



//async
static PyObject *cf_EditArea_GetContent(PyObject *self, PyObject *args){
    char* absolutepath;
    if(!PyArg_ParseTuple(args, "s", &absolutepath)){
        return nullptr;
    }

    static request::EAGetText req;
    req.Filepath = absolutepath;

    std::future<const result::Result*> Gettext = std::async(core::Interact, &req);
    const result::GetText* r = (result::GetText*)Gettext.get();
    const std::string* text = r->Text;

    if(text->empty()){
        Py_RETURN_NONE;
    }else{
        return PyUnicode_FromString(text->c_str());
    }
    Py_RETURN_NONE;
}


static PyObject *cf_EditArea_AddCallBack(PyObject *self, PyObject *args){
    char *filename;
    char *eventtype;
    char *pyfuncname;

    if(!PyArg_ParseTuple(args, "zss", &filename, &eventtype, &pyfuncname)){
        Py_RETURN_NONE;
    }

    static request::EAAddCallBack req;

    if(strcmp(eventtype, "OPENNEW")==0){
        //
        req.Filepath = "";
        req.Type = request::EAAddCallBack::NEWEDITAREA;
    }else if(strcmp(eventtype, "TEXTCHANGED")==0){
        //
        req.Filepath = filename;
        req.Type = request::EAAddCallBack::TEXTCHANGED;
    }else{
        Py_RETURN_NONE;
    }

    req.Funcname = pyfuncname;

    core::Interact(&req);
    Py_RETURN_NONE;
}

static PyObject *cf_EditArea_RemoveCallBack(PyObject *self, PyObject *args){
    char *absolutepath, *pyfuncname;
    if(!PyArg_ParseTuple(args, "ss", &absolutepath, &pyfuncname)){
        return nullptr;
    }
    /*
    */
    Py_RETURN_NONE;
}


static PyObject *cf_EditArea_HighLight(PyObject *self, PyObject *args){
    char *absolutepath, *tagname;
    int line, offset, length;
    if(!PyArg_ParseTuple(args, "siiis", &absolutepath,&line,&offset,&length,&tagname)){
        return nullptr;
    }

    static request::EADrawByLine req;
    req.Filepath = absolutepath;
    req.Line = line;
    req.Offset = offset;
    req.Length = length;
    req.Tagname = tagname;

    core::Interact(&req);

    Py_RETURN_NONE;
}

static PyMethodDef cf_EditArea_method[]={
    {"test", cf_Test, METH_VARARGS, ""},
    {"getcontent", cf_EditArea_GetContent, METH_VARARGS, "get content from edit area"},
    {"addcallback", cf_EditArea_AddCallBack, METH_VARARGS, "add callback"},
    {"rmcallback", cf_EditArea_RemoveCallBack, METH_VARARGS, "remove callback"},
    {"highlight", cf_EditArea_HighLight, METH_VARARGS, "highlight line(>= 1) pos(>= 1) length(>= 1) with tagname"},
    {"setlanguage", cf_EditArea_SetLang, METH_VARARGS, "set the language of edit area"},
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
    .m_name = "CloudForestPy",
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
