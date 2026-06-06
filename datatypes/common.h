#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include <string>
#include <unordered_set>

/*
 * Zero based
 */
typedef unsigned int ZNumber;

typedef struct ZPosition{
    ZNumber line;
    ZNumber column;
} ZPosition;

typedef struct ZRange{
    char direction;//-1 for negative, 0 for not changed, 1 for positive
    ZPosition start;
    ZPosition end;
} ZRange;

/*
 * One based
 */
typedef unsigned int ONumber;

typedef struct OPosition{
    ZNumber line;
    ZNumber column;
} OPosition;

typedef struct ORange{
    bool direction;
    OPosition start;
    OPosition end;
} ORange;

typedef struct Difference{
    ZRange before;
    ZRange after;
    char* text;
} Difference;

typedef void(*EventCallback)();// void (*)()


/*
 * Be careful. The "language" here is programming language.
 */

typedef struct Language {
    std::string name;
    std::string id;
    std::unordered_set<std::string> fileExtensions;
    std::string syntaxTemplateFile;
} Language;

typedef struct Extension {
    const char* name;
    const char* folder;
    const char* description;
    bool enabled;
} Extension;

#endif
