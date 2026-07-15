#include "language_mod_Py.h"

#include "datatypes/common.h"
#include "src/gui/editarea/EditArea.h"
#include "src/languages/LanguageGroup.h"
#include "src/languages/LanguageListener.h"
#include "src/languages/LanguageManager_if.h"
#include "src/session/EditAreaData.h"
#include "python_tool.h"
#include "editarea/editarea_mod_Py.h"
#include "src/session/SessionEvent.h"
#include "toolset/event/Event.h"
#include "toolset/tools/Tool.h"

#include <abstract.h>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>

#include <memory>
#include <methodobject.h>
#include <modsupport.h>
#include <object.h>
#include <pytypedefs.h>
#include <setobject.h>
#include <tupleobject.h>
#include <unicodeobject.h>

static PythonEventPtrMap lang_new_ea_event_map;
static PythonEventPtrMap lang_used_callbacks_dict;

void OnLanguageUsed(const Language* lang){
    RestoreThreadLock();
    auto itr = lang_used_callbacks_dict.find(lang->name);
    if(itr == lang_used_callbacks_dict.end()){
        return;
    }
    PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(lang->name));
    itr->second->Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

void OnNewEditArea(EditArea* ea){
    RestoreThreadLock();
    auto itr = lang_new_ea_event_map.find(ea->GetLanguage()->name);

    if(itr == lang_new_ea_event_map.end()){
        return;
    }

    py_EditArea *py_ea = ea->GetPyEditArea();
    if (py_ea) {
        PyObject* args = PyTuple_Pack(1, py_ea);
        itr->second->Invoke(args);
    }
    ReleaseThreadLock();
}


static PyObject *language_module_clear_data(PyObject *self, PyObject *args){
    Py_RETURN_NONE;
}

static PyObject *language_module_add_language(PyObject *self, PyObject *args){
    char* langname;
    char* id;
    char* syntaxfile;
    char* fileextension;
    if(!PyArg_ParseTuple(args, "ssss", &langname, &id, &syntaxfile, &fileextension)){
        Py_RETURN_NAN;
    }

    std::unique_ptr<Language> lang = std::make_unique<Language>();// Free by lang manager
    lang->name = strdup(langname);
    lang->id = strdup(id);
    lang->syntaxTemplateFile = syntaxfile;

    auto extlist = tools::TrimText(fileextension, "[],' ");
    for(auto ext : extlist){
        lang->fileExtensions.emplace(ext);
    }

    langmanager::AddToLanguageList(std::move(lang));

    Py_RETURN_NONE;
}

static PyObject *language_module_listen_language_used(PyObject *self, PyObject* args){
    char* langname;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        Py_RETURN_NAN;
    }

    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    auto itr = lang_used_callbacks_dict.find(langname);
    if (itr != lang_used_callbacks_dict.end()) {
        itr->second->Connect(callback);
    } else {
        std::unique_ptr<PythonEvent> event = std::make_unique<PythonEvent>();
        event->Connect(callback);
        lang_used_callbacks_dict.emplace(langname, std::move(event));
    }

    Py_RETURN_NONE;
}


static PyObject *language_module_listen_for_editarea(PyObject *self, PyObject *args){
    char* langname;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    PythonEvent* event;

    auto itr = lang_new_ea_event_map.find(langname);
    if (itr == lang_new_ea_event_map.end()) {
        std::unique_ptr<PythonEvent> newevent = std::make_unique<PythonEvent>();
        event = newevent.get();
        lang_new_ea_event_map.emplace(langname, std::move(newevent));
    } else {
        event = itr -> second.get();
    }

    event->Connect(callback);

    Language* lang = langmanager::FindByName(langname);
    LanguageGroup* langgroup = session::FindLanguageGroup(lang);

    if(langgroup != nullptr){
        langgroup->Listen(LanguageGroup::ADDED, (EventCallback)OnNewEditArea);
    }

    Py_RETURN_NONE;
}

static PyObject *language_module_stop_listen_for_editarea(PyObject *self, PyObject *args){
    char* langname;
    PyObject* callback;

    if(!PyArg_ParseTuple(args, "sO", &langname, &callback)){
        Py_RETURN_NAN;
    }
    if(!PyCallable_Check(callback)){
        Py_RETURN_NAN;
    }

    Language* lang = langmanager::FindByName(langname);
    LanguageGroup* langgroup = session::FindLanguageGroup(lang);

    if(langgroup != nullptr){
        langgroup->StopListen(LanguageGroup::ADDED, (EventCallback)OnNewEditArea);
    }

    Py_RETURN_NONE;
}


static PyMethodDef language_module_method[] = {
    {"clear_data",  language_module_clear_data, METH_VARARGS,"clear language datas. call this before reloading the language panel"},
    {"add_language",  language_module_add_language, METH_VARARGS,"add a language to the language list."},
    {"listen_language_used", language_module_listen_language_used, METH_VARARGS, "the language used for the first time"},
    {"listen_for_editarea", language_module_listen_for_editarea, METH_VARARGS, "listen for new editarea with specific language"},
    {"stop_listen_for_editarea", language_module_stop_listen_for_editarea, METH_VARARGS, "stop listen for new editarea with specific language"},
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef language_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "language",
    .m_size = 0,
    .m_methods = language_module_method,
    .m_slots = nullptr
};

PyMODINIT_FUNC PyInit_language_module(){
    PyObject *langmodule = PyModule_Create(&language_module);
    session::Listen(session::LANGUAGE_USED, (EventCallback) OnLanguageUsed);
    return langmodule;
}
