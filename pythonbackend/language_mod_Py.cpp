#include "language_mod_Py.h"

#include "pythonbackend/python_if.h"
#include "src/AppContext.h"
#include "src/languages/LanguageManager.h"
#include "src/gui/editarea/EditArea.h"
#include "src/languages/Language.h"
#include "src/languages/LanguageManager.h"
#include "python_tool.h"
#include "editarea/editarea_mod_Py.h"
#include "toolset/event/Event.h"
#include "toolset/tools/Tool.h"

#include <abstract.h>
#include <dictobject.h>
#include <floatobject.h>
#include <listobject.h>
#include <longobject.h>
#include <memory>
#include <methodobject.h>
#include <modsupport.h>
#include <object.h>
#include <pythonrun.h>
#include <pytypedefs.h>
#include <setobject.h>
#include <tupleobject.h>
#include <unicodeobject.h>

static PythonEventPtrMap lang_new_ea_event_map;
static PythonEventPtrMap lang_used_callbacks_dict;

void on_new_language_added(Language* lang){
    //
}

void on_language_used(Language* lang){
    if (!TryRestoreThreadLock()) {
        return;
    }
    auto itr = lang_used_callbacks_dict.find(lang->GetInfo()->name);
    if(itr == lang_used_callbacks_dict.end()){
        return;
    }
    PyObject* args = PyTuple_Pack(1, PyUnicode_FromString(lang->GetInfo()->name.c_str()));
    itr->second->Invoke(args);
    Py_DECREF(args);
    ReleaseThreadLock();
}

void on_new_edit_area(EditArea* ea){
    auto itr = lang_new_ea_event_map.find(ea->GetLanguage()->GetInfo()->name);
    if(itr == lang_new_ea_event_map.end()){
        return;
    }

    if (!TryRestoreThreadLock()) {
        return;
    }

    py_EditArea *py_ea = ea->GetPyEditArea();

    if (py_ea) {
        PyObject* args = PyTuple_Pack(1, py_ea);
        itr->second->Invoke(args);
        Py_DECREF(args);
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

    std::unique_ptr<LanguageInfo> info = std::make_unique<LanguageInfo>();// Free by lang manager
    info->name = strdup(langname);
    info->id = strdup(id);
    info->syntaxTemplateFile = syntaxfile;

    auto extlist = tools::TrimText(fileextension, "[],' ");
    for(auto ext : extlist){
        info->fileExtensions.emplace(ext);
    }

    Language& lang = pybackend::GetAppContext().langManager->CreateLanguage(std::move(info));
    lang.Listen(Language::LANG_FIRST_USED, (EventCallback)on_language_used);

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

    Language* lang = pybackend::GetAppContext().langManager->FindByName(langname);

    if(lang != nullptr){
        lang->Listen(Language::EDITAREA_ADDED, (EventCallback)on_new_edit_area);
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

    Language* lang = pybackend::GetAppContext().langManager->FindByName(langname);

    if(lang != nullptr){
        lang->StopListen(Language::EDITAREA_ADDED, (EventCallback)on_new_edit_area);
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
    pybackend::GetAppContext().langManager->Listen(LanguageManager::LANG_MANAGER_NEW_LANG, (EventCallback) on_new_language_added);
    return langmodule;
}
