#include "pyEditArea.h"
#include <Python.h>
#include <abstract.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterobject.h>
#include <listobject.h>
#include <longobject.h>
#include <methodobject.h>
#include <modsupport.h>
#include <moduleobject.h>
#include <object.h>
#include <pyport.h>
#include <tupleobject.h>
#include <unicodeobject.h>
#include "../Core.h"

#include "../requests/GUIRequests.h"

class EditArea; //forward definition

typedef struct {
    PyObject_HEAD;
    char* Filepath;
    char* Lang;
    EditArea *Editarea;
    PyObject *TextchangedCallbacks;
    PyObject *RequestCompletionCallbacks;
    PyObject *LangChangedCallbacks;
    PyObject *FileSavedCallbacks;
    PyObject *FilePathAndNameChangedCallbacks;
}py_EditArea;

static PyObject *py_EditArea_GetFilePath(py_EditArea *self, PyObject *args){
    PyObject *name = PyUnicode_FromString(self->Filepath);
    return name;
}

static PyObject *py_EditArea_SetLang(py_EditArea *self, PyObject *args){
    char* absolutepath, lang;
    if(!PyArg_ParseTuple(args, "ss", &absolutepath, lang)){
        return nullptr;
    }

    static EASetLang req;
    req.Parent = self->Editarea;
    req.Lang = lang;

    core::Interact(&req);

    Py_RETURN_NONE;
}

//async
static PyObject *py_EditArea_GetContent(py_EditArea *self, PyObject *args){
    static EAGetText req;
    req.Parent = self->Editarea;

    std::future<const result::Result*> Gettext = std::async(core::Interact, &req);
    const result::GetText* r = (result::GetText*)Gettext.get();
    std::string text = r->Text;

    if(text.empty()){
        Py_RETURN_NONE;
    }else{
        return PyUnicode_FromString(text.c_str());
    }
    Py_RETURN_NONE;
}


static PyObject *py_EditArea_AddCallBack(py_EditArea *self, PyObject *args){
    char *eventtype;
    PyObject *func;

    if(!PyArg_ParseTuple(args, "sO", &eventtype, &func)){
        Py_RETURN_NONE;
    }

    if(!PyCallable_Check(func)){
        return nullptr;
    }

    if(strcmp(eventtype, "TEXTCHANGED")==0){
        // Text changed
        PyList_Append(self->TextchangedCallbacks, func);
    }else if(strcmp(eventtype, "REQUESTCOMPLETION")== 0){
        // Request auto completion from language server
        PyList_Append(self->RequestCompletionCallbacks, func);
    }else if(strcmp(eventtype, "LANGCHANGED")==0){
        // Language changed from EditArea. Not from file extension
        PyList_Append(self->LangChangedCallbacks, func);
    }else if(strcmp(eventtype, "FILESAVED")==0){
        PyList_Append(self->FileSavedCallbacks, func);
    }else{
        Py_RETURN_NONE;
    }

    Py_RETURN_NONE;
}

static PyObject *py_EditArea_RemoveCallBack(py_EditArea *self, PyObject *args){
    char *eventtype;
    PyObject *func;

    if(!PyArg_ParseTuple(args, "sO",&eventtype,&func)){
        return nullptr;
    }

    if(!PyCallable_Check(func)){
        return nullptr;
    }

    PyObject *callbacklist;
    if(strcmp(eventtype, "TEXTCHANGED")){
        callbacklist = self->TextchangedCallbacks;
    }if(strcmp(eventtype, "REQUESTCOMPLETION")){
        callbacklist = self->RequestCompletionCallbacks;
    }else{
        return nullptr;
    }

    for (Py_ssize_t itr = 0; itr < PyList_GET_SIZE(callbacklist); itr++) {
        PyObject *function = PyList_GetItem(callbacklist, itr);
        if(function == func){
            PyList_SetItem(callbacklist, itr, nullptr);
            Py_DecRef(func);
        }
    }
    /*
    */
    Py_RETURN_NONE;
}


static PyObject *py_EditArea_HighLight(py_EditArea *self, PyObject *args){
    char *absolutepath, *tagname;
    int line, offset, length;
    if(!PyArg_ParseTuple(args, "iiis",&line,&offset,&length,&tagname)){
        return nullptr;
    }

    static EADrawByLine req;
    req.Parent = self->Editarea;
    req.Line = line;
    req.Offset = offset;
    req.Length = length;
    req.Tagname = tagname;

    core::Interact(&req);

    Py_RETURN_NONE;
}


static PyObject* py_EditArea_AddSuggestion(py_EditArea *self, PyObject *args){
    char *absolutepath, *suggestion, *label;
    unsigned int startline, startpos, endline, endpos;
    if(!PyArg_ParseTuple(args, "ssiiii",
        &suggestion,&label,&startline,&startpos, &endline, &endpos)){
        return nullptr;
    }

    static EAAddSuggestion req;
    Suggestion sug;
    sug.InsertText = suggestion;
    sug.Label = label;
    sug.Range.StartLine = startline;
    sug.Range.StartChar = startpos;
    sug.Range.EndLine = endline;
    sug.Range.EndChar = endpos;

    req.Sug = &sug;
    req.Parent = self->Editarea;
    /*
     * [!NOTE]
     * the suggestuon object will be moved to the suggestion popover,
     */

    core::Interact(&req);

    Py_RETURN_NONE;
}

static PyObject* py_EditArea_ClearSuggestion(py_EditArea *self, PyObject *args){
    static EAClearSuggestion req;
    req.Parent = self->Editarea;
    core::Interact(&req);
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_ShowSuggestion(py_EditArea *self, PyObject *args){
    static EAShowSuggestion req;
    req.Parent = self->Editarea;
    core::Interact(&req);
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_HideSuggestion(py_EditArea *self, PyObject *args){
    static EAHideSuggestion req;
    req.Parent = self->Editarea;
    core::Interact(&req);
    Py_RETURN_NONE;
}

static PyMethodDef cf_EditArea_class_method[]={
    {"getfilepath", (PyCFunction)py_EditArea_GetFilePath, METH_VARARGS, "get the file path"},
    {"getcontent", (PyCFunction)py_EditArea_GetContent, METH_VARARGS, "get content from edit area"},
    {"addcallback", (PyCFunction)py_EditArea_AddCallBack, METH_VARARGS, "add callback"},
    {"rmcallback", (PyCFunction)py_EditArea_RemoveCallBack, METH_VARARGS, "remove callback"},
    {"highlight", (PyCFunction)py_EditArea_HighLight, METH_VARARGS, "highlight line(>= 1) pos(>= 1) length(>= 1) with tagname"},
    {"setlanguage", (PyCFunction)py_EditArea_SetLang, METH_VARARGS, "set the language of edit area"},
    {"addsuggestion", (PyCFunction)py_EditArea_AddSuggestion, METH_VARARGS, "add a suggestion to the autocomplete"},
    {"clearsuggestion", (PyCFunction)py_EditArea_ClearSuggestion, METH_VARARGS, "clear the suggestions of the edit area"},
    {"hidesuggestion", (PyCFunction)py_EditArea_HideSuggestion, METH_VARARGS, "hide the suggestion popover"},
    {"showsuggestion", (PyCFunction)py_EditArea_ShowSuggestion, METH_VARARGS, "show the suggestion popover"},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject cf_EditArea_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "EditArea",
    .tp_basicsize = sizeof(py_EditArea),
    .tp_doc = "Edit area of the CloudForest",
    .tp_methods = cf_EditArea_class_method,
    .tp_new = PyType_GenericNew
};

PyTypeObject* init_cf_EditArea_class(){
    if(PyType_Ready(&cf_EditArea_class)<0){
        g_print("cannot build edit area python type\n");
        return nullptr;
    }
    return &cf_EditArea_class;
}




/*
 * This is the module
 */

// lists
static PyObject *RegisteredEditAreas = nullptr;

static PyObject *EARegisteredCallbackList = nullptr;
static PyObject *EALanguageChangedCallbackList = nullptr;

// tools
static PyObject *FindEditAreaForPython(EditArea *ea){
    for(size_t itr = 0; itr < PyList_GET_SIZE(RegisteredEditAreas); itr++){
        py_EditArea *registeredea = (py_EditArea*)PyList_GET_ITEM(RegisteredEditAreas, itr);
        if(registeredea->Editarea == ea){
            return (PyObject*)registeredea;
        }
    }
    return nullptr;
}

static PyObject *cf_EditArea_module_addcallback(PyObject *self, PyObject *args){
    char* event;
    PyObject* callbackfunc;
    if(!PyArg_ParseTuple(args, "sO", &event, &callbackfunc)){
        return nullptr;
    }
    if(!PyCallable_Check(callbackfunc)){
        return nullptr;
    }


    if (strcmp(event, "NEWEDITAREA")==0) {
        PyList_Append(EARegisteredCallbackList, callbackfunc);
    } else if(strcmp(event, "LANGUAGECHANGED")==0){
        PyList_Append(EALanguageChangedCallbackList, callbackfunc);
    }

    Py_RETURN_NONE;
}

static PyMethodDef cf_EditArea_module_method[]={
    {"addcallback", cf_EditArea_module_addcallback, METH_VARARGS, "add callback for event"},
    {nullptr, nullptr, 0, nullptr}
};

static PyModuleDef cf_EditArea_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "EditAreaMod",
    .m_size = 0,
    .m_methods = cf_EditArea_module_method,
    .m_slots = nullptr,
};

PyMODINIT_FUNC init_cf_EditArea_module(){
    RegisteredEditAreas = PyList_New(0);
    EARegisteredCallbackList = PyList_New(0);

    PyObject *eamodule = PyModule_Create(&cf_EditArea_module);

    PyModule_AddObject(eamodule, "EditArea", (PyObject*)init_cf_EditArea_class());
    return eamodule;
}




/*
 * This is the functions exposed to the .h file
 */

void register_py_EditArea(PyRegisterEA *req){
    /*
     * When the gui creates a EditArea, we have to create an
     * instance of the cf_EditArea, so the python extension can
     * manipulate the edit area
     */

    py_EditArea *newEa =(py_EditArea*)PyObject_CallObject((PyObject*)&cf_EditArea_class, nullptr);
    PyList_Append(RegisteredEditAreas, (PyObject*)newEa);
    newEa->Editarea = req->ea;
    newEa->Filepath = strdup(req->FilePath.c_str());
    newEa->TextchangedCallbacks = PyList_New(0);
    newEa->RequestCompletionCallbacks = PyList_New(0);
    newEa->FileSavedCallbacks = PyList_New(0);
    newEa->LangChangedCallbacks = PyList_New(0);


    // we are trying to make filepath a variable
    PyObject *filepath = PyUnicode_FromString(req->FilePath.c_str());

    PyObject *args = PyTuple_Pack(1, newEa);
    for(unsigned int itr = 0; itr < PyList_Size(EARegisteredCallbackList); itr++){
        //call the callbacks
        PyObject *func = PyList_GetItem(EARegisteredCallbackList, itr);

        PyObject_CallObject(func, args);
    }

    Py_DECREF(filepath);
    Py_DECREF(args);
}

void callback_py_EditArea_object(PyCallbackEA *req){
    py_EditArea *pyeditarea = (py_EditArea*)FindEditAreaForPython(req->ea);

    if(req->m_CallbackType == PyCallbackEA::TEXTCHANGED){
        PyObject *args = PyTuple_Pack(1, pyeditarea);

        for (Py_ssize_t itr = 0; itr < PyList_Size(pyeditarea->TextchangedCallbacks); itr++) {
            PyObject_CallObject(PyList_GetItem(pyeditarea->TextchangedCallbacks, itr),args);
        }

        Py_DECREF(args);
    }else if(req->m_CallbackType == PyCallbackEA::REQUESTCOMPLETION){
        PyObject *args = PyTuple_Pack(3,
            pyeditarea,
            PyLong_FromUnsignedLong(req->m_StartLine),
            PyLong_FromUnsignedLong(req->m_StartPos));

        for (Py_ssize_t itr = 0; itr < PyList_Size(pyeditarea->RequestCompletionCallbacks); itr++) {
            PyObject_CallObject(PyList_GetItem(pyeditarea->RequestCompletionCallbacks, itr),args);
        }

        Py_DECREF(args);
    }
}
