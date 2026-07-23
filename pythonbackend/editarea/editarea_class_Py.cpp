#include "editarea_class_Py.h"

#include "editarea_mod_Py.h"
#include "pythonbackend/python_tool.h"
#include "editarea/DiagnosticTool.h"
#include "editarea/EditArea.h"
#include "src/languages/LanguageManager_if.h"
#include "datatypes/lsp.h"
#include "datatypes/common.h"
#include "toolset/event/Event.h"

#include <Python.h>
#include <cpython/classobject.h>
#include <cstdio>
#include <cstring>
#include <floatobject.h>
#include <listobject.h>
#include <longobject.h>
#include <memory>
#include <methodobject.h>
#include <object.h>
#include <pytypedefs.h>

/*
 * Callbacks
 */

static void OnEditAreaClosed(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    const Difference &dif = ea->GetPendingDiff();
    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_CLOSED);
    event.Invoke(args);
    Py_DECREF(args);
    Py_DECREF(py_ea);
    ReleaseThreadLock();
}

static void OnEditAreaCompletionRequested(EditArea *ea, const ZPosition& zpos){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(3, py_ea, PyLong_FromLong(zpos.line), PyLong_FromLong(zpos.column));
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_COMPLETION_REQUESTED);
    event.Invoke(args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

static void OnEditAreaCursorMoved(EditArea *ea, const ZPosition &pos){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(3, py_ea, PyLong_FromLong(pos.line), PyLong_FromLong(pos.column));
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_CURSOR_MOVED);
    event.Invoke(args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

static void OnEditAreaFileDataChanged(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_FILE_DATA_CHANGED);
    event.Invoke(args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

static void OnEditAreaFileSaved(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_FILE_SAVED);
    event.Invoke(args);
    Py_DECREF((PyObject*) args);
    ReleaseThreadLock();
}

static void OnEditAreaLangChanged(EditArea *ea, Language* oldlang, Language* newlang){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_LANG_CHANGED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static void OnEditAreaTextChanged(EditArea *ea){
    RestoreThreadLock();
    py_EditArea* py_ea = ea->GetPyEditArea();
    if(py_ea == nullptr) return;
    Py_INCREF(py_ea);
    const Difference &dif = ea->GetPendingDiff();
    PyObject* args = PyTuple_Pack(4, py_ea, GetPyDictFromZRange(dif.before), PyUnicode_FromString(dif.text), PyLong_FromLong(ea->GetFileVersion()));
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_TEXT_CHANGED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}


/*
 * Functions
 */

static PyObject *py_EditArea_get_file_path(py_EditArea *self, PyObject *args){
    PyObject* path = PyUnicode_FromString(self->filePath);
    return path;
}

static PyObject *py_EditArea_get_file_version(py_EditArea *self, PyObject *args){
    PyObject* version = PyLong_FromUnsignedLong(self->editarea->GetFileVersion());
    return version;
}

static PyObject *py_EditArea_get_lang(py_EditArea *self, PyObject *args){
    std::string text = self->editarea->GetLanguage()->name;

    if(text.empty()){
        Py_RETURN_NAN;
    }else{
        return PyUnicode_FromString(text.c_str());
    }
    Py_RETURN_NONE;
}

static PyObject *py_EditArea_set_lang(py_EditArea *self, PyObject *args){
    char* lang;
    if(!PyArg_ParseTuple(args, "s", &lang)){
        Py_RETURN_NAN;
    }

    self->editarea->SetLanguage(langmanager::FindByName(lang));

    Py_RETURN_NONE;
}


static PyObject *py_EditArea_get_content(py_EditArea *self, PyObject *args){
    const char* text = self->editarea->GetContent();
    return PyUnicode_FromString(text);
}

static PyObject *py_EditArea_add_callback(py_EditArea *self, PyObject *args){
    char *event;
    PyObject *callback;

    if(!PyArg_ParseTuple(args, "sO", &event, &callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    auto itr = self->eventMap->find(event);
    if (itr != self->eventMap->end()){
        itr->second.Connect(callback);
    }

    Py_RETURN_NONE;
}

static PyObject *py_EditArea_rm_callback(py_EditArea *self, PyObject *args){
    static unsigned int rm_id = 0;
    char *event;
    PyObject *callback;

    if(!PyArg_ParseTuple(args, "sO",&event,&callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    auto itr = self->eventMap->find(event);
    if (itr != self->eventMap->end()){
        itr->second.Disconnect(callback);
    }

    Py_RETURN_NONE;
}


static PyObject *py_EditArea_highlight(py_EditArea *self, PyObject *args){
    char *tagname;
    int line, offset, length;
    if(!PyArg_ParseTuple(args, "siii",&tagname,&line,&offset,&length)){
        Py_RETURN_NAN;
    }

    self->editarea->ApplyTagByLinePos(line, offset, length, tagname);
    Py_RETURN_NONE;
}


static PyObject* py_EditArea_add_completion(py_EditArea *self, PyObject *args){
    char *absolutepath, *text, *label;
    unsigned int startline, startpos, endline, endpos;
    if(!PyArg_ParseTuple(args, "ssiiii",
        &text, &label,&startline,&startpos, &endline, &endpos)){
            Py_RETURN_NAN;
    }

    std::unique_ptr<Completion> comp = std::make_unique<Completion>();
    comp->insertText = text;
    comp->label = label;
    comp->range.start.line = startline;
    comp->range.start.column = startpos;
    comp->range.end.line = endline;
    comp->range.end.column = endpos;

    self->editarea->GetCompletionTool().Add(std::move(comp));

    Py_RETURN_NONE;
}

static PyObject* py_EditArea_clear_completion(py_EditArea *self, PyObject *args){
    self->editarea->GetCompletionTool().Clear();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_show_completion(py_EditArea *self, PyObject *args){
    self->editarea->GetCompletionTool().ShowPopover();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_hide_completion(py_EditArea *self, PyObject *args){
    self->editarea->GetCompletionTool().HidePopover();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_add_diagnostic(py_EditArea *self, PyObject *args){
    /*
     * The message cannot be paseds directly to diagnostic as Python will free the char*
     */

    std::unique_ptr<Diagnostic> diagnostic = std::make_unique<Diagnostic>();
    char* message;
    char* code;
    if(!PyArg_ParseTuple(
        args, "ssiiiii",
        &code,
        &message,
        &diagnostic->range.start.line,
        &diagnostic->range.start.column,
        &diagnostic->range.end.line,
        &diagnostic->range.end.column,
        &diagnostic->severity)
    ){
        Py_RETURN_NAN;
    }

    diagnostic->message = strdup(message);
    diagnostic->code = strdup(code);
    self->editarea->GetDiagnosticTool().Add(std::move(diagnostic));
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_process_diagnostics(py_EditArea* self, PyObject *args){
    int version;
    if(!PyArg_ParseTuple(args, "i", &version)){
        Py_RETURN_NAN;
    }
    self->editarea->GetDiagnosticTool().Process(version);
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_clear_diagnostics(py_EditArea* self, PyObject *args){
    self->editarea->GetDiagnosticTool().Clear();
    Py_RETURN_NONE;
}

static PyMethodDef py_EditArea_class_method[]={
    {"get_file_path", (PyCFunction)py_EditArea_get_file_path, METH_VARARGS, "get the file path"},
    {"get_file_version", (PyCFunction)py_EditArea_get_file_version, METH_VARARGS, "get the version of the file"},
    {"get_content", (PyCFunction)py_EditArea_get_content, METH_VARARGS, "get content from edit area"},
    {"add_callback", (PyCFunction)py_EditArea_add_callback, METH_VARARGS, "add callback"},
    {"rm_callback", (PyCFunction)py_EditArea_rm_callback, METH_VARARGS, "remove callback"},
    {"highlight", (PyCFunction)py_EditArea_highlight, METH_VARARGS, "highlight line(>= 0) pos(>= 0) length(>= 0) with tagname"},
    {"set_language", (PyCFunction)py_EditArea_set_lang, METH_VARARGS, "set the language of edit area"},
    {"get_language", (PyCFunction)py_EditArea_get_lang, METH_VARARGS, "get the language of edit area"},
    {"add_completion", (PyCFunction)py_EditArea_add_completion, METH_VARARGS, "add a suggestion to the autocomplete"},
    {"clear_completion", (PyCFunction)py_EditArea_clear_completion, METH_VARARGS, "clear the suggestions of the edit area"},
    {"hide_completion", (PyCFunction)py_EditArea_hide_completion, METH_VARARGS, "hide the suggestion popover"},
    {"show_completion", (PyCFunction)py_EditArea_show_completion, METH_VARARGS, "show the suggestion popover"},
    {"add_diagnostic", (PyCFunction)py_EditArea_add_diagnostic, METH_VARARGS, "add diagnostic to EditArea"},
    {"process_diagnostics", (PyCFunction)py_EditArea_process_diagnostics, METH_VARARGS, "read diagnostics and apply tags in the EditArea"},
    {"clear_diagnostics", (PyCFunction)py_EditArea_clear_diagnostics, METH_VARARGS, "clear all diagnostics in the EditArea"},
    {NULL, NULL, 0, NULL}
};

static PyObject* py_EditArea_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
    py_EditArea *self = (py_EditArea*)type->tp_alloc(type, 0);

    self->eventMap = new PythonEventMap();// freed on aedit area closed

    self->eventMap->emplace(PY_EDITAREA_EVENT_CLOSED, PythonEvent());
    self->eventMap->emplace(PY_EDITAREA_EVENT_COMPLETION_REQUESTED, PythonEvent());
    self->eventMap->emplace(PY_EDITAREA_EVENT_CURSOR_MOVED, PythonEvent());
    self->eventMap->emplace(PY_EDITAREA_EVENT_FILE_DATA_CHANGED, PythonEvent());
    self->eventMap->emplace(PY_EDITAREA_EVENT_FILE_SAVED, PythonEvent());
    self->eventMap->emplace(PY_EDITAREA_EVENT_LANG_CHANGED, PythonEvent());
    self->eventMap->emplace(PY_EDITAREA_EVENT_TEXT_CHANGED, PythonEvent());

    return (PyObject *) self;
}

PyTypeObject py_EditArea_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "EditArea",
    .tp_basicsize = sizeof(py_EditArea),
    .tp_doc = "Edit area of the CloudForest",
    .tp_methods = py_EditArea_class_method,
    .tp_new = py_EditArea_new,
};

void py_EditArea_connect_events(py_EditArea* py_ea){
    //
    EditArea* ea = py_ea->editarea;
    ea->Listen(EditArea::CLOSED, (EventCallback)OnEditAreaClosed);
    ea->Listen(EditArea::COMPLETION_REQUESTED, (EventCallback)OnEditAreaCompletionRequested);
    ea->Listen(EditArea::CURSOR_MOVED, (EventCallback)OnEditAreaCursorMoved);
    ea->Listen(EditArea::FILE_DATA_CHANGED, (EventCallback)OnEditAreaFileDataChanged);
    ea->Listen(EditArea::FILE_SAVED, (EventCallback)OnEditAreaFileSaved);
    ea->Listen(EditArea::LANG_CHANGED, (EventCallback)OnEditAreaLangChanged);
    ea->Listen(EditArea::TEXT_CHANGED, (EventCallback)OnEditAreaTextChanged);
}

py_EditArea* py_EditArea_create_object(EditArea* ea){
    RestoreThreadLock();
    py_EditArea* py_ea = (py_EditArea*)PyObject_CallObject((PyObject*)&py_EditArea_class, nullptr);
    py_ea->editarea = ea;
    py_ea->filePath = strdup(ea->GetFilePath());
    ReleaseThreadLock();
    return py_ea;
}

PyTypeObject* PyInit_py_EditArea_class(){
    if(PyType_Ready(&py_EditArea_class)<0){
        g_print("cannot build edit area python type\n");
        return nullptr;
    }
    return &py_EditArea_class;
}
