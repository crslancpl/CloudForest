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

bool StartWith(const string &Text, const string &Pattern){
    if(Text.length() < Pattern.length()) return false;
    for(int i = 0; i < Pattern.length(); i++){
        if(Text[i] != Pattern[i])return false;
    }
    return true;
}

bool EndWith(const string &Text, const string &Pattern){
    if(Text.length() < Pattern.length()) return false;
    for(int i = 0; i < Pattern.length(); i++){
        if(Text[Text.length()-1 - i] != Pattern[Pattern.length()-1 - i])return false;
    }
    return true;
}

vector<string> TrimText(const string &Text, const string &Separator) {
    vector<string> result;
    string current;

    for (char c : Text) {
        if (Separator.find(c) != string::npos) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
	    }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}
