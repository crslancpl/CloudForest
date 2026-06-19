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
