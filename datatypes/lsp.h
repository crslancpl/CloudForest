#ifndef LSP_DATA_H_
#define LSP_DATA_H_

#include "common.h"

#include <string>
#include <unordered_set>

enum class COMPLETION_ITEM_KIND{
    TEXT = 1,
    METHOD = 2,
    FUNCTION = 3,
    CONSTRUCTOR = 4,
    FIELD = 5,
    VARIABLE = 6,
    CLASS = 7,
    INTERFACE = 8,
    MODULE = 9,
    PROPERTY = 10,
    UNIT = 11,
    VALUE = 12,
    ENUM = 13,
    KEYWORD = 14,
    SNIPPET = 15,
    COLOR = 16,
    FILE = 17,
    REFERENCE = 18,
    FOLDER = 19,
    ENUM_MEMBER = 20,
    CONSTANT = 21,
    STRUCT = 22,
    EVENT = 23,
    OPERATOR = 24,
    TYPE_PARAMETER = 25,
};

typedef struct Completion{
    std::string label;// text to be shown on suggestion
    std::string insertText;
    std::string description;
    short itemKind;
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
