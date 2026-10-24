#pragma once

#include "MessageReader.h"
#include "ChildProcess.h"

#include <string>

class LspClient : public ChildProcess {
public:
    LspClient(const std::string& command, const std::string& langid);
    ~LspClient();

    pid_t StartServer();
    void EndServer();

    void Initialize();

    void SendMessage(const std::string& msg);

    int ReadContentLength();
    std::string ReadLine();

protected:
    void listenOutput();

    LSPMessageReader reader;

private:
    bool stopRequested = false;
    std::string lspCommand;
    std::string langId;
};
