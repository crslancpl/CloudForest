#ifndef TOOL_H_
#define TOOL_H_

#include "datatypes/common.h"


#include <string>
#include <vector>

namespace tools{
int GetIntDigitCount(int num);
std::string GenerateId();
bool StartWith(const std::string &text, const std::string &pattern);
bool EndWith(const std::string &text, const std::string &pattern);
std::vector<std::string> TrimText(const std::string &text, const std::string &separator);

bool IsLineIndexInRange(const unsigned int line, const unsigned int index, const Range* range);
bool IsRangeInRange(const Range* input, const Range* range);
}

#endif
