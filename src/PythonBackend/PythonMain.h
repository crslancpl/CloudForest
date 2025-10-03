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
const result::Result* Process(Request* request);
};

#endif
