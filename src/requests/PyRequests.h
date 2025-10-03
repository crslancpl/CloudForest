#ifndef PYREQUEST_H_
#define PYREQUEST_H_

#include "BaseRequest.h"

#include <string>

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

class PyRunCallBack:Request{
public:
    enum CallbackType {
        NEWEDITAREA
    };

    CallbackType Type;
    PyRunCallBack(): Request(Parts::PYTHON){}
};

#endif
