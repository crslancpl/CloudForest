#include "text_tool.h"


#include <cstdio>
#include <cstdlib>
#include <cstring>


int tools::GetIntDigitCount(int num){
    int count = 1;
    while (num/10 > 0) {
        count ++;
        num %= 10;
    }
    return count;
}

unsigned int num = 0;

std::string tools::GenerateId(){
    num++;
    return std::to_string(num);
}

bool tools::StartWith(const std::string &text, const std::string &pattern){
    if(text.length() < pattern.length()) return false;
    for(int i = 0; i < pattern.length(); i++){
        if(text[i] != pattern[i])return false;
    }
    return true;
}

bool tools::EndWith(const std::string &text, const std::string &pattern){
    if(text.length() < pattern.length()) return false;
    for(int i = 0; i < pattern.length(); i++){
        if(text[text.length() - 1 - i] != pattern[pattern.length() - 1 - i]) return false;
    }
    return true;
}

std::vector<std::string> tools::TrimText(const std::string &text, const std::string &separator) {
    std::vector<std::string> result;
    std::string current;

    for (char c : text) {
        if (separator.find(c) != std::string::npos) {
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
