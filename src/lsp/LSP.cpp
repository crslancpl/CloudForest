#include "LSP.h"
#include "../Global.h"
#include "../Core.h"

#include <cstdio>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <utility>

static std::vector<std::pair<std::string,LSPServer*>> Servers;

LSPServer* LSPServer::GetServer(const std::string &lspname){
    for(std::pair<std::string, LSPServer*> &server: Servers){
        if(server.first == lspname){
            return server.second;
        }
    }

    Servers.emplace_back(lspname, new LSPServer());
    LSPServer* server = Servers.back().second;
    server->Init(lspname);
    return server;
}

void LSPServer::StopAll(){
    for (auto &server: Servers) {
        server.second->Stop();
    }
}

void LSPServer::Init(const std::string &lspname) {
    Msg.Init();
    const result::Result* res = core::Interact(&Msg);
    if (res == nullptr) {
        return;
    }

    if (pipe(LSPPipe) < 0) {
        perror("pipe");
        exit(1);
    }

    int processid = fork();
    if (processid < 0) {
        perror("fork");
        exit(1);
    }


    if (processid == 0) {
        // Child process
        // close(LSPPipe[1]); // Close write end
        dup2(LSPPipe[0], STDIN_FILENO); // Redirect pipe read end to stdin
        close(LSPPipe[0]); // Close original read end

        execlp(lspname.c_str(), lspname.c_str(), nullptr);
        perror("execlp"); // If execlp fails
    } else {
        // Parent process
        close(LSPPipe[0]); // Close read end

        Msg.Init();
        std::future<const result::Result*> result = std::async(core::Interact, &Msg);
        result::GetText* text = (result::GetText*) result.get();

        Send(text->Text);
    }
}

void LSPServer::Autocomplete(const std::string &filepath,unsigned int line, unsigned int pos){
    Msg.AutoComplete(filepath, line, pos);

    std::future<const result::Result*> gettext = std::async(core::Interact, &Msg);
    result::GetText* text = (result::GetText*)gettext.get();

    std::string message = text->Text;

    Send(message);
}

void LSPServer::Stop(){
    //sent exit message to server
    Msg.Exit();
    std::future<const result::Result*> gettext = std::async(core::Interact, &Msg);
    result::GetText* text = (result::GetText*)gettext.get();
    Send(text->Text);
    close(LSPPipe[1]); // Close write end
}

void LSPServer::DidOpen(const std::string &filepath, const std::string &fileContent){
    Msg.ChangeContent(filepath, fileContent);
    std::future<const result::Result*> gettext = std::async(core::Interact, &Msg);
    result::GetText* text = (result::GetText*)gettext.get();

    std::string message = text->Text;

    Send(message);
}

void LSPServer::Send(const std::string& message){
    snprintf(ContentLengthMes, sizeof(ContentLengthMes), "Content-Length: %zu\r\n\r\n", message.length());

    if (write(LSPPipe[1], ContentLengthMes, strlen(ContentLengthMes)) < 0) {
       perror("write");
    }
    if (write(LSPPipe[1], message.c_str(), message.length()) < 0) {
       perror("write");
    }
}
