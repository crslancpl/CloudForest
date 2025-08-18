#include "ToolFunctions.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

int GetIntDigitCount(int num){
    int count = 1;
    while (num/10 > 0) {
        count ++;
        num %= 10;
    }
    return count;
}

unsigned int num = 0;

void GenerateId(string& stringtogenerate){
    num++;
    stringtogenerate = to_string(num);
}
