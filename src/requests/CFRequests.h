#ifndef CFREQUEST_H_
#define CFREQUEST_H_

#include "BaseRequest.h"

#include <string>

class CFLoadTemplate: public Request{
public:
    std::string Language;
    void (*Callback)();
    CFLoadTemplate() : Request(Parts::CLOUDYFOREST) {}
};

class CFHighlight: public Request{
public:
    std::string Filepath;
    std::string Language;
    CFHighlight() : Request(Parts::CLOUDYFOREST) {}
};

class CFReadFile: public Request{
public:
    std::string Filepath;
    std::string Language;
    CFReadFile() : Request(Parts::CLOUDYFOREST) {}
};

#endif
