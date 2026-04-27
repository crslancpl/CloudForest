#ifndef SUGGESTION_H_
#define SUGGESTION_H_

#include <string>

typedef struct SuggestionRange{
    unsigned int startLine, startColumn;
    unsigned int endLine, endColumn;
}SuggestionRange;

typedef struct Suggestion{
    std::string label;// text to be shown on suggestion
    std::string insertText;
    std::string type;
    std::string description;
    SuggestionRange range;// where to insert the text
}Suggestion;

#endif
