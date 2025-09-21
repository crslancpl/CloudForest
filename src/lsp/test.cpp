#include "lsp.h"

#include <cstdio>
#include <unistd.h>
#include <string>

void LSPBackend::InitServer(const std::string &lspname){
    int lsppipe[2], processid;
    if(pipe(lsppipe) < 0){
        exit(1);
    }

    if((processid = fork()) > 0){
        execlp(lspname.c_str(), "",nullptr);
        std::string initmessage =
            "{\"jsonrpc\": \"2.0\","
            "\"id\": 1,"
            "\"method\":\"initialize\","
            "\"params\":{\"processId\":null, \"rootUri\":\"file:///home\",\"capabilities\":{}"
            "}";
        char contentlengthmes[30];
        sprintf(contentlengthmes, "Content-Length: %i\r\n\r\n", (int)initmessage.length());

        write(lsppipe[1], contentlengthmes, 30);
        write(lsppipe[1], initmessage.c_str(), initmessage.length());
    }
}
