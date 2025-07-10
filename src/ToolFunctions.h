#ifndef TOOLFUNCTIONS_H_
#define TOOLFUNCTIONS_H_
#pragma once

typedef struct Array{
    int size;
    void *Content;
}Array;

void *ArrayGet(Array *arr, unsigned int number);
void ArrayAdd(Array *arr,void *item);
void ArrayRemove(Array *arr);

int GetIntDigitCount(int num);

#endif
