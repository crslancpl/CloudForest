#ifndef LANGUAGE_DATA_H_
#define LANGUAGE_DATA_H_


/*
 * Be careful. The "language" here is programming language.
 */

#include <unordered_set>
#include <string>

typedef struct Language{
    std::string name;
    std::string id;
    std::unordered_set<std::string> fileExtensions;
    std::string syntaxTemplateFile;
}Language;

#endif
