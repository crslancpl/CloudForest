#include "cfEditArea.h"
#include <Python.h>
#include <object.h>
#include "../Core.h"

#include "../requests/GUIRequests.h"

class EditArea; //forward definition

typedef struct {
    PyObject_HEAD
    char* Filepath;
    EditArea *Editarea;
    PyObject *TextchangedCallbacks;
}cf_EditArea;


static PyObject *cf_EditArea_SetLang(cf_EditArea *self, PyObject *args){
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
static PyObject *cf_EditArea_GetContent(cf_EditArea *self, PyObject *args){
    char* absolutepath;
    if(!PyArg_ParseTuple(args, "s", &absolutepath)){
        return nullptr;
    }

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


static PyObject *cf_EditArea_AddCallBack(cf_EditArea *self, PyObject *args){
    char *filename;
    char *eventtype;
    char *pyfuncname;

    if(!PyArg_ParseTuple(args, "zss", &filename, &eventtype, &pyfuncname)){
        Py_RETURN_NONE;
    }

    static EAAddCallBack req;

    if(strcmp(eventtype, "OPENNEW")==0){
        //
        req.Parent = nullptr;
        req.Type = EAAddCallBack::NEWEDITAREA;
    }else if(strcmp(eventtype, "TEXTCHANGED")==0){
        //
        req.Parent = self->Editarea;
        req.Type = EAAddCallBack::TEXTCHANGED;
    }else{
        Py_RETURN_NONE;
    }

    req.Funcname = pyfuncname;

    core::Interact(&req);
    Py_RETURN_NONE;
}

static PyObject *cf_EditArea_RemoveCallBack(cf_EditArea *self, PyObject *args){
    char *absolutepath, *pyfuncname;
    if(!PyArg_ParseTuple(args, "ss", &absolutepath, &pyfuncname)){
        return nullptr;
    }
    /*
    */
    Py_RETURN_NONE;
}


static PyObject *cf_EditArea_HighLight(cf_EditArea *self, PyObject *args){
    char *absolutepath, *tagname;
    int line, offset, length;
    if(!PyArg_ParseTuple(args, "siiis", &absolutepath,&line,&offset,&length,&tagname)){
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


static PyObject* cf_EditArea_AddSuggestion(cf_EditArea *self, PyObject *args){
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

static PyObject* cf_EditArea_ClearSuggestion(cf_EditArea *self, PyObject *args){
    static EAClearSuggestion req;

    core::Interact(&req);

    Py_RETURN_NONE;
}

static PyMethodDef cf_EditArea_method[]={
    {"getcontent", (PyCFunction)cf_EditArea_GetContent, METH_VARARGS, "get content from edit area"},
    {"addcallback", (PyCFunction)cf_EditArea_AddCallBack, METH_VARARGS, "add callback"},
    {"rmcallback", (PyCFunction)cf_EditArea_RemoveCallBack, METH_VARARGS, "remove callback"},
    {"highlight", (PyCFunction)cf_EditArea_HighLight, METH_VARARGS, "highlight line(>= 1) pos(>= 1) length(>= 1) with tagname"},
    {"setlanguage", (PyCFunction)cf_EditArea_SetLang, METH_VARARGS, "set the language of edit area"},
    {"addsuggestion", (PyCFunction)cf_EditArea_AddSuggestion, METH_VARARGS, "add a suggestion to the autocomplete"},
    {"clearsuggestion", (PyCFunction)cf_EditArea_ClearSuggestion, METH_VARARGS, "clear the suggestions of the edit area"},
    {NULL, NULL, 0, NULL}
};

static PyTypeObject cf_EditArea_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "CloudForestPy.EditArea",
    .tp_basicsize = sizeof(cf_EditArea),
    .tp_doc = "Edit area of the CloudForest",
    .tp_methods = cf_EditArea_method,
    .tp_new = PyType_GenericNew
};

PyTypeObject* init_cf_EditArea_class(){
    if(PyType_Ready(&cf_EditArea_class)<0){
        g_print("cannot build edit area python type\n");
        return nullptr;
    }
    return &cf_EditArea_class;
}
