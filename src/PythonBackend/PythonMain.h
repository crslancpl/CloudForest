#ifndef PYTHONMAIN_H_
#define PYTHONMAIN_H_
#pragma once

#include <string>
#include <Python.h>

using namespace std;

namespace pybackend{
void Start();
void End();
void Execute(const string &code);
void ExecuteFile(const string &path);
};

#endif
