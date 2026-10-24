#pragma once

#include <stack>

class Content {};

class BaseContent : public Content{
    //
};

class LSPMessageReader {
public:

    void SetFileDescriptor(int fd);
    void Read(int length);

protected:
    int readFd = 0; // read from std in
    std::stack<Content> contentStack;
};
