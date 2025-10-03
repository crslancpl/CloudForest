#include "cfPythonTool.h"
#include <abstract.h>
#include <cstddef>
#include <import.h>
#include <modsupport.h>
#include <moduleobject.h>
#include <object.h>
#include <string>
#include <unicodeobject.h>

PyObject *modulename;
PyObject *LSPMsgModule;

void InitPythonTool(){
    modulename = PyUnicode_FromString("extension.CloudForestBuiltIn.LSPMsg");
    LSPMsgModule = PyImport_Import(modulename);
    Py_DECREF(modulename);
}

const result::GetText* GetLspMessage(PyGetLspMessage* req){
    /*
     * [NOTE]
     * The python code is in extension/CloudForestBuiltIn/LSPMsg.py
     */

    static result::GetText Message;
    static std::string text;
    static PyObject *function;
    static PyObject* value;

    if(LSPMsgModule == nullptr){
        g_print("cfPythonTool GetLspMessage: cannot find module\n");
        return nullptr;
    }


    if(req->MessageType == PyGetLspMessage::INIT){
        function = PyObject_GetAttrString(LSPMsgModule, "GetInitMessage");
        if(PyCallable_Check(function)){
            value = PyObject_CallObject(function, NULL);
        }
    }else if(req->MessageType == PyGetLspMessage::EXIT){
        g_print("exit\n");
        function = PyObject_GetAttrString(LSPMsgModule, "GetExitMessage");
        if(PyCallable_Check(function)){
            value = PyObject_CallObject(function, NULL);
        }
    }else if(req->MessageType == PyGetLspMessage::CHANGECONTENT){
        function = PyObject_GetAttrString(LSPMsgModule, "GetDidOpenMessage");
        if(PyCallable_Check(function)){
            PyObject* param = Py_BuildValue("(ss)",req->Path.c_str(), req->Content.c_str());
            value = PyObject_CallObject(function, param);
        }
    }else if(req->MessageType == PyGetLspMessage::AUTOCOMPLETE){
        function = PyObject_GetAttrString(LSPMsgModule, "GetAutoCompMessage");
        if(PyCallable_Check(function)){
            PyObject* param = Py_BuildValue("(sii)",req->Path.c_str(), req->Line, req->Pos);
            value = PyObject_CallObject(function, param);
        }
    }else{
        return nullptr;
    }

    text = PyUnicode_AsUTF8(value);
    Message.Text = text;
    return &Message;
}
