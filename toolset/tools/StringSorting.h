#ifndef STRINGSORTING_H_
#define STRINGSORTING_H_

#include <string>

enum CompareResult{
    COMPARE_RESULT_GREATER,
    COMPARE_RESULT_EQUAL,
    COMPARE_RESULT_LESS
};

inline CompareResult CompareChar(char c1, char c2);

inline CompareResult CompareChar(char c1, char c2){
    char lowerc1, lowerc2;
    if (c1 > 'A' && c1 < 'Z'){
        lowerc1 = c1 - 'A' + 'a';
    } else {
        lowerc1 = c1;
    }

    if (c2 > 'A' && c2 < 'Z'){
        lowerc2 = c2 - 'A' + 'a';
    } else {
        lowerc2 = c2;
    }

    /*
     * Different characters:
     * Here 'a' is the greatest, 'z' is the smallest
     */

    if(lowerc1 > lowerc2){
        return COMPARE_RESULT_LESS;
    }else if(lowerc1 < lowerc2){
        return COMPARE_RESULT_GREATER;
    }


    /*
     * Same character:
     * Upper is greater than lower
     */
    if(c1 < c2){
        return COMPARE_RESULT_LESS;
    }else if(c1 > c2){
        return COMPARE_RESULT_GREATER;
    }

    return COMPARE_RESULT_EQUAL;
}

CompareResult CompareString(const std::string& s1, const std::string& s2);

unsigned long int CalculateSortingCode(const char* str);


#endif
