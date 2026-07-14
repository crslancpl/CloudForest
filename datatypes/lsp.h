#ifndef LSP_DATA_H_
#define LSP_DATA_H_

#include "common.h"

#include <string>
#include <unordered_set>


typedef struct Completion{
    std::string label;// text to be shown on suggestion
    std::string insertText;
    std::string type;
    std::string description;
    ZRange range;// where to insert the text
}Completion;


typedef struct Diagnostic{
    char* code;
    char* message;
    int severity;
    ZRange range;
}Diagnostic;

typedef std::unordered_set<Diagnostic*> DiagnosticList;

#endif
