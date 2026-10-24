#include "MessageReader.h"

void LSPMessageReader::SetFileDescriptor(int fd){
    readFd = fd;
}

void LSPMessageReader::Read(int length){
    for (int i = 0; i < length; i++) {
        //
    }
}
