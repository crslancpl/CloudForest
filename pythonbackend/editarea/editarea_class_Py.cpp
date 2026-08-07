#include "editarea_class_Py.h"

#include "editarea/CompletionTool.h"
#include "editarea_mod_Py.h"
#include "pythonbackend/python_tool.h"
#include "editarea/DiagnosticTool.h"
#include "editarea/EditArea.h"
#include "src/languages/LanguageManager_if.h"
#include "datatypes/lsp.h"
#include "datatypes/common.h"
#include "toolset/event/Event.h"

#include <Python.h>
#include <abstract.h>
#include <cpython/classobject.h>
#include <cstdio>
#include <cstring>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>
#include <longobject.h>
#include <memory>
#include <methodobject.h>
#include <modsupport.h>
#include <object.h>
#include <pytypedefs.h>
#include <string>
#include <sysmodule.h>
#include <unicodeobject.h>
#include <list>

/*
 * Callbacks
 */

static void on_edit_area_closed(EditArea *ea){
    py_EditArea* py_ea = ea->GetPyEditArea();
    if (py_ea == nullptr) {
        return;
    }
    if (!TryRestoreThreadLock()) {
        return;
    }

    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_CLOSED);
    event.Invoke(args);
    Py_DECREF(args);
    Py_DECREF(py_ea);

    ReleaseThreadLock();
}

static void on_edit_area_completion_requested(EditArea *ea, const ZPosition& zpos){
    py_EditArea* py_ea = ea->GetPyEditArea();
    if (py_ea == nullptr) {
        return;
    }
    if (!TryRestoreThreadLock()) {
        return;
    }
    PauseExtensionCall();

    PyObject* args = PyTuple_Pack(3, py_ea, PyLong_FromLong(zpos.line), PyLong_FromLong(zpos.column));
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_COMPLETION_REQUESTED);
    event.Invoke(args);
    Py_DECREF(args);

    ReleaseThreadLock();
    ResumeExtensionCall();
}

static void on_edit_area_cursor_moved(EditArea *ea, const ZPosition &pos){
    py_EditArea* py_ea = ea->GetPyEditArea();
    if (py_ea == nullptr) {
        return;
    }
    if (!TryRestoreThreadLock()) {
        return;
    }

    PyObject* args = PyTuple_Pack(3, py_ea, PyLong_FromLong(pos.line), PyLong_FromLong(pos.column));
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_CURSOR_MOVED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static void on_edit_area_file_data_changed(EditArea *ea){
    py_EditArea* py_ea = ea->GetPyEditArea();
    if (py_ea == nullptr) {
        return;
    }
    if (!TryRestoreThreadLock()) {
        return;
    }

    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_FILE_DATA_CHANGED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static void on_edit_area_file_saved(EditArea *ea){
    py_EditArea* py_ea = ea->GetPyEditArea();
    if (py_ea == nullptr) {
        return;
    }
    if (!TryRestoreThreadLock()) {
        return;
    }

    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_FILE_SAVED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static void on_edit_area_lang_changed(EditArea *ea, const Language* oldlang, const Language* newlang){
    py_EditArea* py_ea = ea->GetPyEditArea();
    if (py_ea == nullptr) {
        return;
    }
    if (!TryRestoreThreadLock()) {
        return;
    }

    PyObject* args = PyTuple_Pack(1, py_ea);
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_LANG_CHANGED);
    event.Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

static void on_edit_area_text_changed(EditArea *ea, const Difference& diff){
    static std::list<Difference> pendingdiffs;
    py_EditArea* py_ea = ea->GetPyEditArea();

    if (py_ea == nullptr) {
        return;
    }

    pendingdiffs.emplace_back(diff);

    if (!TryRestoreThreadLock()) {
        return;
    }
    PauseExtensionCall();

    if (pendingdiffs.empty()) {
        ReleaseThreadLock();
        ResumeExtensionCall();
        return;
    }

    PyObject* contentchanges = PyList_New(0);
    for (const Difference& diff: pendingdiffs) {
        PyObject* item = PyDict_New();

        PyDict_SetItemString(item, "range", GetPyDictFromZRange(diff.before));
        PyDict_SetItemString(item, "text", PyUnicode_FromString(diff.text.c_str()));
        PyList_Append(contentchanges, item);
    }

    PyObject* args = PyTuple_Pack(3, py_ea, contentchanges, PyLong_FromLong(ea->GetFileVersion()));
    PythonEvent &event = py_ea->eventMap->at(PY_EDITAREA_EVENT_TEXT_CHANGED);

    event.Invoke(args);
    Py_DECREF(args);

    pendingdiffs.clear();

    ReleaseThreadLock();
    ResumeExtensionCall();
}




/*
 * Functions
 */

static PyObject *py_EditArea_get_file_path(py_EditArea *self, PyObject *args){
    PyObject* path = PyUnicode_FromString(self->editarea->GetFilePath());
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


static PyObject* py_EditArea_show_completion(py_EditArea *self, PyObject *args){
    PyObject *result;
    if (GetIsExtensionPausedCall()) {
        Py_RETURN_NONE;
    }

    if (!PyArg_ParseTuple(args, "O", &result)) {
        Py_RETURN_NAN;
    }
    CompletionTool& comptool = self->editarea->GetCompletionTool();
    PyObject *items = PyDict_GetItemString(result, "items");//list

    if (!PyList_Check(items)) {
        Py_RETURN_NAN;
    }

    BlockRestoringThreadLock();
    int itemcount = PyList_GET_SIZE(items);
    comptool.Clear();
    if (itemcount == 0) {
        comptool.HidePopover();
        AllowRestoringThreadLock();
        Py_RETURN_NONE;
    }

    for (int itr = 0; itr < itemcount; itr++) {
        std::unique_ptr<Completion> completion = std::make_unique<Completion>();
        PyObject* item = PyList_GetItem(items, itr);//dict
        PyObject* label = PyDict_GetItemString(item, "label");
        completion->label = PyUnicode_AsUTF8(label);

        PyObject* inserttext = PyDict_GetItemString(item, "insertText");
        if (inserttext) {
            completion->insertText = PyUnicode_AsUTF8(inserttext);
        }

        PyObject* kind = PyDict_GetItemString(item, "kind");
        if (kind) {
            completion->itemKind = PyLong_AsLong(kind);
        }
        PyObject* documentation = PyDict_GetItemString(item, "documentation");
        if (documentation) {
            if (PyDict_Check(documentation)) {
                // markup content
                PyObject* kind = PyDict_GetItemString(documentation, "kind");
                std::string dockind = PyUnicode_AsUTF8(kind);
                PyObject* value = PyDict_GetItemString(documentation, "value");
                if (dockind == "plaintext") {
                    completion->doc = PyUnicode_AsUTF8(value);
                }
            } else {
                completion->doc = PyUnicode_AsUTF8(documentation);
            }
        }

        {
            PyObject* textedit = PyDict_GetItemString(item, "textEdit");
            PyObject* range = PyDict_GetItemString(textedit, "range");
            completion->range = GetZRangeFromPyDict(range);
        }
        comptool.Add(std::move(completion));
    }

    comptool.ShowPopover();
    AllowRestoringThreadLock();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_clear_completion(py_EditArea *self, PyObject *args){
    self->editarea->GetCompletionTool().Clear();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_hide_completion(py_EditArea *self, PyObject *args){
    self->editarea->GetCompletionTool().HidePopover();
    Py_RETURN_NONE;
}

static PyObject* py_EditArea_process_diagnostics(py_EditArea *self, PyObject *args){
    PyObject *diagnostics;// list
    int version;
    if (GetIsExtensionPausedCall()) {
        Py_RETURN_NONE;
    }

    if (!PyArg_ParseTuple(args, "Oi", &diagnostics, &version)) {
        Py_RETURN_NAN;
    }

    if (!PyList_Check(diagnostics)) {
        printf("editarea_class_Py: diagnostics not a list\n");
        Py_RETURN_NAN;
    }

    DiagnosticTool& diagtool = self->editarea->GetDiagnosticTool();
    diagtool.Clear();
    int currentversion = self->editarea->GetFileVersion();
    if (version != currentversion) {
        printf("editarea_class_Py: wrong version, current %i %i\n", currentversion, version);
        Py_RETURN_NONE;
    }

    int itemcount = PyList_GET_SIZE(diagnostics);
    for (int i = 0; i < itemcount; i++) {
        std::unique_ptr<Diagnostic> diagnostic = std::make_unique<Diagnostic>();
        PyObject* item = PyList_GetItem(diagnostics, i);//dict
        PyObject* range = PyDict_GetItemString(item, "range");
        diagnostic->range = GetZRangeFromPyDict(range);
        PyObject* code = PyDict_GetItemString(item, "code");
        if (code) {
            diagnostic->code = PyUnicode_AsUTF8(code);
        }
        PyObject* message = PyDict_GetItemString(item, "message");
        diagnostic->message = PyUnicode_AsUTF8(message);
        PyObject* severity = PyDict_GetItemString(item, "severity");
        diagnostic->severity = PyLong_AsLong(severity);
        diagtool.Add(std::move(diagnostic));
    }

    diagtool.Process(version);
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
    {"show_completion", (PyCFunction)py_EditArea_show_completion, METH_VARARGS, "param: 'result' of lsp as a dict"},
    {"clear_completion", (PyCFunction)py_EditArea_clear_completion, METH_VARARGS, "clear the suggestions of the edit area"},
    {"hide_completion", (PyCFunction)py_EditArea_hide_completion, METH_VARARGS, "hide the suggestion popover"},
    {"process_diagnostic", (PyCFunction)py_EditArea_process_diagnostics, METH_VARARGS, "param: 1.diagnostics as dict, 2.version as int"},
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

    return (PyObject*) self;
}

static void py_EditArea_dealloc(PyObject* self){
    //printf("py_EditArea_dealloc called\n");
    py_EditArea* py_ea = (py_EditArea*) self;
    delete py_ea->eventMap;
}

PyTypeObject py_EditArea_class = {
    .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "EditArea",
    .tp_basicsize = sizeof(py_EditArea),
    .tp_dealloc = py_EditArea_dealloc,
    .tp_doc = "Edit area of the CloudForest",
    .tp_methods = py_EditArea_class_method,
    .tp_new = py_EditArea_new,
};

void py_EditArea_connect_events(py_EditArea* py_ea){
    //
    EditArea* ea = py_ea->editarea;
    ea->Listen(EditArea::CLOSED, (EventCallback)on_edit_area_closed);
    ea->Listen(EditArea::COMPLETION_REQUESTED, (EventCallback)on_edit_area_completion_requested);
    ea->Listen(EditArea::CURSOR_MOVED, (EventCallback)on_edit_area_cursor_moved);
    ea->Listen(EditArea::FILE_DATA_CHANGED, (EventCallback)on_edit_area_file_data_changed);
    ea->Listen(EditArea::FILE_SAVED, (EventCallback)on_edit_area_file_saved);
    ea->Listen(EditArea::LANG_CHANGED, (EventCallback)on_edit_area_lang_changed);
    ea->Listen(EditArea::TEXT_CHANGED, (EventCallback)on_edit_area_text_changed);
}

py_EditArea* py_EditArea_create_object(EditArea* ea){
    TryRestoreThreadLock();
    py_EditArea* py_ea = (py_EditArea*)PyObject_CallObject((PyObject*)&py_EditArea_class, nullptr);
    py_ea->editarea = ea;
    ReleaseThreadLock();
    return py_ea;
}

PyTypeObject* PyInit_py_EditArea_class(){
    if(PyType_Ready(&py_EditArea_class)<0){
        printf("editarea_class_Py: cannot build edit area python type\n");
        return nullptr;
    }
    return &py_EditArea_class;
}
