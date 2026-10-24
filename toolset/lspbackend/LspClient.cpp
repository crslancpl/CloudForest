#include "LspClient.h"

#include "messages/Base.h"

#include <string>
#include <sys/poll.h>
#include <thread>
#include <unistd.h>
#include <cstdio>

LspClient::LspClient(const std::string& command, const std::string& langid) :
    lspCommand(command),
    langId(langid)
{
    //
}

LspClient::~LspClient(){
    //
}

int LspClient::StartServer(){
    //StartReading();
    readingThread = std::thread(&LspClient::listenOutput, this);
    pid_t id = Start(lspCommand);
    fprintf(stderr, "(stderr) start server\n");
    return id;
}

void LspClient::EndServer(){
    stopRequested = true;
}

void LspClient::Initialize(){
    std::string init_message = "{\"jsonrpc\": \"2.0\",\"id\": 1,\"method\": \"initialize\",\"params\": {}}";
    SendMessage(init_message);
    return;
}

void LspClient::SendMessage(const std::string& msg){
    std::string content_length = "Content-Length: " + std::to_string(msg.length()) + "\r\n\r\n";
    write(toChildFd, content_length.c_str(), content_length.length());
    write(toChildFd, msg.c_str(), msg.length());
    write(toChildFd, "\r\n", 2);
}

std::string LspClient::ReadLine(){
    char c = 0;
    std::string str;
    while (true) {
        c = fgetc(fromChild);

        if (c == '\r') {
            continue;
        } else if (c == '\n') {
            return str;
        }

        str += c;
    }
}

int LspClient::ReadContentLength(){
    char c = 0;
    int index = 0;
    const char* contentLengthHeader = "Content-Length: ";
    while (true) {
        c = fgetc(fromChild);

        if (c != contentLengthHeader[index]) {
            // is not content length header
            index = 0;

        }

        index ++;
    }
}

void LspClient::listenOutput(){
    while (true) {
        if (!fromChild) {
            continue;
        }

        std::string str = ReadLine();// should be "Content-Length: number \r\n"

        ReadLine();// "\r\n"

        BaseMessage bm(fromChild);
        //reader.Read(content_length);
    }
}
