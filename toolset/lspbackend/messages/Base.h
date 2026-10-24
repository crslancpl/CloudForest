#pragma once

#include <string>

class LspParams {
    //
};

class BaseMessage {
public:
    int id;
    std::string method;
    LspParams* params;

    BaseMessage(FILE* fromServer);
};
