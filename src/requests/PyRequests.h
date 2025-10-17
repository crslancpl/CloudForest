#ifndef PYREQUEST_H_
#define PYREQUEST_H_

#include "BaseRequest.h"

#include <string>

class EditArea;

class PyRunCode: public Request{
public:
    std::string Code;
    PyRunCode() : Request(Parts::PYTHON) {}
};

class PyRunFile: public Request{
public:
    std::string Filepath;
    PyRunFile() : Request(Parts::PYTHON) {}
};

class PyOpenLanguageServer: public Request{
    public:
    char *LanguageServerCommand, *CommandOption;
    PyOpenLanguageServer() : Request(Parts::PYTHON){}
};

class PyGetLspMessage: public Request{
public:
    /*
     * result: result::GetText
     *
     * use Init(), AutoComplete(), Exit()... to change the type
     * of the lsp message
     */

    enum Type{
        INIT, AUTOCOMPLETE, CHANGECONTENT, EXIT
    };

    void Init();
    void AutoComplete(const std::string &path, unsigned int line, unsigned int pos);
    void ChangeContent(const std::string &path, const std::string &content);
    void Exit();
    Type MessageType;
    std::string Path;
    std::string Content;
    int Line, Pos;

    PyGetLspMessage(): Request(Parts::PYTHON){}
};

class PyRunCallBack:public Request{
public:
    enum CallbackType {
        NEWEDITAREA
    };

    CallbackType Type;
    PyRunCallBack(): Request(Parts::PYTHON){}
};

class PyRegisterEA:public Request{
public:
    EditArea *ea;
    std::string FilePath;
    PyRegisterEA(): Request(Parts::PYTHON){}
};

class PyCallbackEA:public Request{
public:
    enum CallbackType{
        TEXTCHANGED, REQUESTCOMPLETION
    };
    EditArea *ea;
    CallbackType m_CallbackType;
    unsigned int m_StartLine, m_StartPos;
    PyCallbackEA(): Request(Parts::PYTHON){}
};

#endif
