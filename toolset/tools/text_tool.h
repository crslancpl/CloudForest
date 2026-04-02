#ifndef TEXT_TOOL_H_
#define TEXT_TOOL_H_
#pragma once

#include <string>
#include <vector>

namespace tools{
int GetIntDigitCount(int num);
std::string GenerateId();
bool StartWith(const std::string &text, const std::string &pattern);
bool EndWith(const std::string &text, const std::string &pattern);
std::vector<std::string> TrimText(const std::string &text, const std::string &separator);
}

#endif
