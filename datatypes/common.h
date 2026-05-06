#ifndef COMMON_H_
#define COMMON_H_

typedef struct Range{
    unsigned int startLine, startColumn;
    unsigned int endLine, endColumn;
}Range;

typedef void(*EventCallback)();// void (*)()

#endif
