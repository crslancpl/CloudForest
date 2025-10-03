#include "PyRequests.h"

void PyGetLspMessage::Init(){
    MessageType = INIT;
}

void PyGetLspMessage::AutoComplete(const std::string &path, unsigned int line, unsigned int pos){
    MessageType = AUTOCOMPLETE;
    Path = path;
    Line = line;
    Pos = pos;
}
void PyGetLspMessage::ChangeContent(const std::string &path, const std::string &content){
    MessageType = CHANGECONTENT;
    Path = path;
    Content = content;
}
void PyGetLspMessage::Exit(){
    MessageType = EXIT;
}
