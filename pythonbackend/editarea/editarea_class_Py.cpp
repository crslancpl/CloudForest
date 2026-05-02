#include "editarea_class_Py.h"

#include <Python.h>
#include <pytypedefs.h>


#include "src/gui/editarea/LspPopovers_if.h"
#include "src/gui/editarea/LspPopovers.h"
#include "src/gui/editarea/EditArea.h"
#include "src/languages/LanguageManager_if.h"
#include "datatypes/lsp.h"
#include "datatypes/language.h"


static PyObject *py_EditArea_get_file_path(py_EditArea *self, PyObject *args){
    PyObject* path = PyUnicode_FromString(self->Filepath);
    return path;
}

static PyObject *py_EditArea_get_lang(py_EditArea *self, PyObject *args){
    std::string text = self->Editarea->GetLanguage()->name.c_str();

    if(text.empty()){
        Py_RETURN_NONE;
    }else{
        return PyUnicode_FromString(text.c_str());
    }
    Py_RETURN_NONE;
}

static PyObject *py_EditArea_set_lang(py_EditArea *self, PyObject *args){
    char* lang;
    if(!PyArg_ParseTuple(args, "s", &lang)){
        return nullptr;
    }

    self->Editarea->SetLanguage(langmanager::FindLanguage(lang));

    Py_RETURN_NONE;
}

static PyObject *py_EditArea_get_content(py_EditArea *self, PyObject *args){
    const char* text = self->Editarea->GetContent();
    return PyUnicode_FromString(text);
    Py_RETURN_NONE;
}


static PyObject *py_EditArea_add_callback(py_EditArea *self, PyObject *args){
    char *eventtype;
    PyObject *func;

    if(!PyArg_ParseTuple(args, "sO", &eventtype, &func)){
        Py_RETURN_NONE;
    }

    if(!PyCallable_Check(func)){
        return nullptr;
    }

    PyObject* callbacklist = PyDict_GetItemString(self->CallbackDictionary, eventtype);
    if(callbacklist == nullptr) return nullptr;
    PyList_Append(callbacklist, func);
    Py_RETURN_NONE;
}

static PyObject *py_EditArea_remove_callback(py_EditArea *self, PyObject *args){
    char *eventtype;
    PyObject *func;

    if(!PyArg_ParseTuple(args, "sO",&eventtype,&func)){
        return nullptr;
    }

    if(!PyCallable_Check(func)){
        return nullptr;
    }

    PyObject *callbacklist = PyDict_GetItemString(self->CallbackDictionary, eventtype);

    if(callbacklist == Py_None) return nullptr;
    return nullptr;
    for (Py_ssize_t itr = 0; itr < PyList_GET_SIZE(callbacklist); itr++) {
        PyObject *function = PyList_GetItem(callbacklist, itr);
        if(function == func){
            PyList_SetItem(callbacklist, itr, nullptr);
            Py_DecRef(func);
        }
    }
    Py_RETURN_NONE;
}


static PyObject *py_EditArea_highlight(py_EditArea *self, PyObject *args){
    char *absolutepath, *tagname;
    int line, offset, length;
    if(!PyArg_ParseTuple(args, "siii",&tagname,&line,&offset,&length)){
        return nullptr;
    }

    self->Editarea->ApplyTagByLinePos(line, offset, length, tagname);
    Py_RETURN_NONE;
}


static PyObject* py_EditArea_add_suggestion(py_EditArea *self, PyObject *args){
    char *absolutepath, *suggestion, *label;
    unsigned int startline, startpos, endline, endpos;
    if(!PyArg_ParseTuple(args, "ssiiii",
        &suggestion,&label,&startline,&startpos, &endline, &endpos)){
        return nullptr;
    }

    Suggestion* sug = new Suggestion();
    sug->insertText = suggestion;
    sug->label = label;
    sug->range.startLine = startline;
    sug->range.startColumn = startpos;
    sug->range.endLine = endline;
    sug->range.endColumn = endpos;

    lsppopovers::suggestion::Add(sug);
    /*
     * [!NOTE]
     * the suggestuon object will be moved to the suggestion popover,
     */

    Py_RETURN_NONE;
}

static PyObject* py_EditArea_clear_suggestion(py_EditArea *self, PyObject *args){
    lsppopovers::suggestion::Clear();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_show_suggestion(py_EditArea *self, PyObject *args){
    lsppopovers::suggestion::Show();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_hide_suggestion(py_EditArea *self, PyObject *args){
    lsppopovers::suggestion::Hide();
    Py_RETURN_NONE;
}

static PyMethodDef py_EditArea_class_method[]={
    {"get_file_path", (PyCFunction)py_EditArea_get_file_path, METH_VARARGS, "get the file path"},
    {"get_content", (PyCFunction)py_EditArea_get_content, METH_VARARGS, "get content from edit area"},
    {"add_callback", (PyCFunction)py_EditArea_add_callback, METH_VARARGS, "add callback"},
    {"rm_callback", (PyCFunction)py_EditArea_remove_callback, METH_VARARGS, "remove callback"},
    {"highlight", (PyCFunction)py_EditArea_highlight, METH_VARARGS, "highlight line(>= 1) pos(>= 1) length(>= 1) with tagname"},
    {"set_language", (PyCFunction)py_EditArea_set_lang, METH_VARARGS, "set the language of edit area"},
    {"get_language", (PyCFunction)py_EditArea_get_lang, METH_VARARGS, "get the language of edit area"},
    {"add_suggestion", (PyCFunction)py_EditArea_add_suggestion, METH_VARARGS, "add a suggestion to the autocomplete"},
    {"clear_suggestion", (PyCFunction)py_EditArea_clear_suggestion, METH_VARARGS, "clear the suggestions of the edit area"},
    {"hide_suggestion", (PyCFunction)py_EditArea_hide_suggestion, METH_VARARGS, "hide the suggestion popover"},
    {"show_suggestion", (PyCFunction)py_EditArea_show_suggestion, METH_VARARGS, "show the suggestion popover"},
    {NULL, NULL, 0, NULL}
};

static PyObject* py_EditArea_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    py_EditArea *self = (py_EditArea*)type->tp_alloc(type, 0);
    self->CallbackDictionary = PyDict_New();
    self->CursorMovedCallbacks = PyList_New(0);
    self->TextchangedCallbacks = PyList_New(0);
    self->CompletionRequestedCallbacks = PyList_New(0);
    self->LangChangedCallbacks = PyList_New(0);
    self->FileSavedCallbacks = PyList_New(0);
    self->FileDataChangedCallbacks = PyList_New(0);

    PyDict_SetItemString(self->CallbackDictionary, "CURSORMOVED", self->CursorMovedCallbacks);
    PyDict_SetItemString(self->CallbackDictionary, "TEXTCHANGED", self->TextchangedCallbacks);
    PyDict_SetItemString(self->CallbackDictionary, "COMPLETIONREQUESTED", self->CompletionRequestedCallbacks);
    PyDict_SetItemString(self->CallbackDictionary, "LANGUAGECHANGED", self->LangChangedCallbacks);
    PyDict_SetItemString(self->CallbackDictionary, "FILESAVED", self->FileSavedCallbacks);
    PyDict_SetItemString(self->CallbackDictionary, "FILEDATACHANGED", self->FileDataChangedCallbacks);
    return (PyObject *) self;
}

PyTypeObject py_EditArea_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "EditArea",
    .tp_basicsize = sizeof(py_EditArea),
    .tp_doc = "Edit area of the CloudForest",
    .tp_methods = py_EditArea_class_method,
    .tp_new = py_EditArea_new
};

py_EditArea* py_EditArea_create_object(){
    return (py_EditArea*)PyObject_CallObject((PyObject*)&py_EditArea_class, nullptr);
}

PyTypeObject* PyInit_py_EditArea_class(){
    if(PyType_Ready(&py_EditArea_class)<0){
        g_print("cannot build edit area python type\n");
        return nullptr;
    }
    return &py_EditArea_class;
}
