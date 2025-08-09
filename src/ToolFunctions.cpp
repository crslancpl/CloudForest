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

int num = 0;

string GenerateId(){
    num++;
    string s = to_string(num);
    return s;
}
