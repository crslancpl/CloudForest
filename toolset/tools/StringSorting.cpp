#include "StringSorting.h"

#include <cstring>
#include <math.h>

unsigned long int CalculateSortingCode(const char* str){
    unsigned long int num = 0;
    int itr = 0;

    for (int i  = 0; i < strlen(str) && i < 4; i++) {
        char c = str[i];
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';//to upper
        }

        num += c* pow(40, 4 - itr) * 0.4;
        itr++;

        if (itr == 4) {
            return num;
        }
    }

    return num;
}

CompareResult CompareString(const std::string& s1, const std::string& s2){
    for (int i = 0; i < s1.length() && i < s2.length(); i++) {
        char char1 = s1[i];
        char char2 = s2[i];

        if (char1 != char2) {
            CompareResult r = CompareChar(char1, char2);
            if (r != COMPARE_RESULT_EQUAL) {
                return r;
            }
        }
    }

    if (s1.length() != s2.length()) {
        return s1.length() < s2.length() ? COMPARE_RESULT_LESS : COMPARE_RESULT_GREATER;
    }

    return COMPARE_RESULT_EQUAL;
}
