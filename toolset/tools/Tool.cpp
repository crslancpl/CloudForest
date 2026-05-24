#include "Tool.h"
#include "datatypes/common.h"


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

bool tools::IsZPosInRange(const ZPosition& pos, const ZRange* range){
    if( pos.line >= range->start.line && pos.line <= range->end.line){
        if(pos.column >= range->start.column && pos.column <= range->end.column){
            return true;
        }
    }
    return false;
}

bool tools::IsZRangeInRange(const ZRange *input, const ZRange *range){
    if( input->start.line >= range->start.line && input->end.line <= range->end.line){
        if(input->start.column >= range->start.column && input->end.column <= range->end.column){
            return true;
        }
    }
    return false;
}

bool tools::IsORangeInRange(const ORange *input, const ORange *range){
    if( input->start.line >= range->start.line && input->end.line <= range->end.line){
        if(input->start.column >= range->start.column && input->end.column <= range->end.column){
            return true;
        }
    }
    return false;
}

ZRange tools::GetZRange(const ZPosition &pos1, const ZPosition &pos2){
    ZRange range;
    if(pos1.line < pos2.line){
        range.start = pos1;
        range.end = pos2;
        range.direction = 1;
    }else if (pos1.line == pos2.line){
        if (pos1.column < pos2.column){
            range.start = pos1;
            range.end = pos2;
            range.direction = 1;
        }else if(pos1.column == pos2.column){
            range.start = pos1;
            range.end = pos2;
            range.direction = 0;
        }else{
            range.start = pos2;
            range.end = pos1;
            range.direction = -1;
        }
    }else {
        range.start = pos2;
        range.end = pos1;
        range.direction = -1;
    }
    return range;
}

void tools::GetZPosFromGtkTextIter(ZPosition &pos, GtkTextIter* itr){
    pos.line = gtk_text_iter_get_line(itr);
    pos.column = gtk_text_iter_get_line_index(itr);
}
