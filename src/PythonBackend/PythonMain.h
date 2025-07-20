#ifndef PYTHONMAIN_H_
#define PYTHONMAIN_H_
#pragma once

#include <string>
#include <python3.10/Python.h>

using namespace std;

class PyBackend{
public:
    void Start();
    void Execute(const string &code);
    void ExecuteFile(const string &path);
};

#endif
