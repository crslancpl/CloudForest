#include "syntax_provider.h"

#include <unordered_map>

#include "src/gui/editarea/EditArea_if.h"
#include "src/gui/editarea/EditArea.h"

#include "CFCore.h"

static std::unordered_map<TextArea*, SyntaxProvider*> ProviderMap;
static std::unordered_map<const char*, SyntaxProvider*> LangToProviderMap;

static void EditAreaCreated(EditArea* ea){
    ea->AddLangChangedCallback(syntaxprovider::SetLanguage);
}

void syntaxprovider::Init(){
    cf::Init();
    editarea::AddNewEditAreaCallback(EditAreaCreated);
}

void syntaxprovider::Highlight(TextArea *ta){
    //ProviderMap.find(ta)->second->Highlight(ta);
}

void syntaxprovider::SetLanguage(TextArea *ta, const char* language){
    auto result = LangToProviderMap.find(language);
    if(result == LangToProviderMap.end()){
        cf::LoadTemplate(language);
    }

    //ProviderMap.insert({ta, })
}

void syntaxprovider::FastHighlight(EditArea* ea){
    cf::ReadFile(ea->getFilePath(), ea->getLanguage().c_str());
}
