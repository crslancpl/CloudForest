#include "SectionData.h"
#include <string.h>


#include "EditArea.h"

SectionData CurrentSection;

void InitSectionData(){
    CurrentSection.EditAreaNum = 0;
}

void RegisterNewEditArea(EditArea *EditAreaPtr){
    CurrentSection.EditAreaNum ++;
}

EditArea* GetEditAreaFromFileName(const char* FileName){
    EditArea *ea = NULL;
    for(int i = 0; i < CurrentSection.EditAreaNum; i ++){
        ea = CurrentSection.AllEditArea+i;
        g_print("%s", ea->FileName);
        if(strcmp(FileName, ea->FileName)){
            break;
        }
    }

    return ea;
}
