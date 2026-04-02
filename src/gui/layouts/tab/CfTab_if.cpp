#include "CfTab_if.h"
#include "CfTabLayout.h"

static CfTabLayout* focused_tablayout;

CfTabLayout* tablayout::NewTabLayout(){
    CfTabLayout *newtablayout = new CfTabLayout();
    focused_tablayout = newtablayout;
    return focused_tablayout;
}

void tablayout::Show(CfContent *page){
    focused_tablayout->Show(page);
}
