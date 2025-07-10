#include "ToolFunctions.h"
#include <gtk/gtk.h>


int GetIntDigitCount(int num){
    int count = 1;
    while (num/10 > 0) {
        count ++;
        num %= 10;
    }
    return count;
}
