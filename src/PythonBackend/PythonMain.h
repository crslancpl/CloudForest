#ifndef PYTHONMAIN_H_
#define PYTHONMAIN_H_
#pragma once

#include <string>
#include <Python.h>
#include "../Core.h"

using namespace std;

namespace pybackend{
void Start();
void End();
void Process(request::Request* request);
};

#endif
