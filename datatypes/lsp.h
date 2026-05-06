#ifndef SUGGESTION_H_
#define SUGGESTION_H_

#include "common.h"

#include <string>


typedef struct Suggestion{
    std::string label;// text to be shown on suggestion
    std::string insertText;
    std::string type;
    std::string description;
    Range range;// where to insert the text
}Suggestion;

typedef struct Diagnostic{
    char* code;
    char* message;
    int severity;
    Range range;
}Diagnostic;

#endif
