#pragma once

#include <mutex>
#include <unistd.h>
#include <string>
#include <thread>

#define READ_END 0
#define WRITE_END 1

class ChildProcess {
public:
    static bool check_supported(); // check current OS
    static bool check_supported_for();

public:
    ChildProcess();
    ~ChildProcess();

    pid_t Start(const std::string& command);// -1 if failed

protected:
    /*
     * Platform dependent data for pipe
     */
    // File descriptions
    int toChildFd;
    FILE* fromChild = nullptr;

    std::thread readingThread;

private:
    int pcFd[2]; // parent to child pipe
    int cpFd[2]; // child to parent pipe

    void runChildProcess(char* args[]);
    void runParentProcess();

    std::string programCommand;
    bool isRunning = false;

    std::mutex processMutex;
};
