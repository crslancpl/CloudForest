#ifndef SECTIONDATA_H_
#define SECTIONDATA_H_
#pragma once

#include "EditArea.h"

typedef struct SectionData{
    EditArea *AllEditArea;
    int EditAreaNum;
}SectionData;

void InitSectionData();
void RegisterNewEditArea(EditArea *EditAreaPtr);
EditArea* GetEditAreaFromFileName(const char* FileName);

#endif
