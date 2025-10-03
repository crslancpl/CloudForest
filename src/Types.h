#ifndef TYPES_H_
#define TYPES_H_

#include <string>

typedef struct{
    unsigned int StartLine, StartChar;
    unsigned int EndLine, EndChar;
}SuggestionRange;

typedef struct{
    std::string Label;// text to be shown on suggestion
    std::string InsertText;
    std::string Type;
    std::string Description;
    SuggestionRange Range;// where to insert the text
}Suggestion;

#endif
