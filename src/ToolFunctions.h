#ifndef TOOLFUNCTIONS_H_
#define TOOLFUNCTIONS_H_
#pragma once

#include <string>
#include <vector>

using namespace std;

int GetIntDigitCount(int num);
void GenerateId(string& stringtogenerate);
bool StartWith(const string &Text, const string &Pattern);
bool EndWith(const string &Text, const string &Pattern);
vector<string> TrimText(const string &Text, const string &Separator);
#endif
