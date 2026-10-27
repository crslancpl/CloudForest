#include "ChildProcess.h"

#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>

ChildProcess::ChildProcess(){}

ChildProcess::~ChildProcess(){
    if (readingThread.joinable()) {
        readingThread.join();
        close(cpFd[READ_END]);
        close(pcFd[WRITE_END]);
    }
}

pid_t ChildProcess::Start(const std::string& command){
    programCommand = command;
    if (pipe(pcFd)) {
        return -1;
    }

    if (pipe(cpFd)) {
        return -1;
    }

    pid_t pid;

    char* args[] = {
        strdup(programCommand.c_str()),
        nullptr
    };

    switch (pid = fork()) {
    case -1:
        close(pcFd[READ_END]);
        close(pcFd[WRITE_END]);
        close(cpFd[READ_END]);
        close(cpFd[WRITE_END]);

        return pid;
    case 0:
        runChildProcess(args);
    }


    runParentProcess();
    return pid;
}


/*
 * Private
 */
void ChildProcess::runChildProcess(char* args[]){
    /*
     * This is the cild process
     */
    close(cpFd[READ_END]);
    close(pcFd[WRITE_END]);

    dup2(cpFd[WRITE_END], 1);
    dup2(pcFd[READ_END], 0);

    //fprintf(stderr, "run child process\n");

    execvp(programCommand.c_str(), args);
}

void ChildProcess::runParentProcess(){
    /*
     * This is the parent process
     */
    close(pcFd[READ_END]);
    close(cpFd[WRITE_END]);

    toChildFd = pcFd[WRITE_END];
    fromChild = fdopen(cpFd[READ_END], "r");

    //fprintf(stderr, "run parent process\n");
}
