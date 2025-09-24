#include "Global.h"

GtkApplication *global::GtkApp = nullptr;

void request::PyGetLspMessage::Init(){
    MessageType = INIT;
}

void request::PyGetLspMessage::AutoComplete(const std::string &path, unsigned int line, unsigned int pos){
    MessageType = AUTOCOMPLETE;
    Path = path;
    Line = line;
    Pos = pos;
}
void request::PyGetLspMessage::ChangeContent(const std::string &path, const std::string &content){
    MessageType = CHANGECONTENT;
    Path = path;
    Content = content;
}
void request::PyGetLspMessage::Exit(){
    MessageType = EXIT;
}
