#ifndef LSP_H_
#define LSP_H_

#include <string>
#include <vector>
#include "../Global.h"
class LSPServer{
public:
    static LSPServer* GetServer(const std::string &lspname);
    static void StopAll();
    static void StopServer(const std::string &lspname);
    int LSPPipe[2];
    char ContentLengthMes[50];
    request::PyGetLspMessage Msg;
    void Init(const std::string &lspname);
    void Autocomplete(const std::string &filepath,unsigned int line, unsigned int pos);
    void DidOpen(const std::string &filepath, const std::string &fileContent);
    void Stop();
private:
    void Send(const std::string& message);
};

#endif
