#ifndef TOOL_H_
#define TOOL_H_

#include "datatypes/common.h"


#include <gtk/gtk.h>
#include <string>
#include <vector>

namespace tools{
int GetIntDigitCount(int num);
std::string GenerateId();
bool StartWith(const std::string &text, const std::string &pattern);
bool EndWith(const std::string &text, const std::string &pattern);
std::vector<std::string> TrimText(const std::string &text, const std::string &separator);

bool IsZPosInRange(const ZPosition &pos, const ZRange* range);
bool IsZRangeInRange(const ZRange* input, const ZRange* range);
bool IsORangeInRange(const ORange* input, const ORange* range);
ZRange GetZRange(const ZPosition &pos1, const ZPosition &pos2);
void GetZPosFromGtkTextIter(ZPosition &pos, GtkTextIter* itr);
}

#endif
