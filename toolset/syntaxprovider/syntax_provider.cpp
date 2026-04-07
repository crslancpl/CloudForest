#include "syntax_provider.h"


#include "src/gui/editarea/EditArea_if.h"
#include "src/gui/editarea/EditArea.h"
#include "CFCore.h"

#include <unordered_map>

static std::unordered_map<TextArea*, SyntaxProvider*> provider_map;
static std::unordered_map<datatypes::Language*, SyntaxProvider*> lang_to_provider_map;

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

void syntaxprovider::SetLanguage(TextArea *ta, datatypes::Language* language){
    auto result = lang_to_provider_map.find(language);
    if(result == lang_to_provider_map.end()){
        cf::LoadTemplate(language->id.c_str());
    }

    //ProviderMap.insert({ta, })
}

void syntaxprovider::FastHighlight(EditArea* ea){
    cf::ReadFile(ea->getFilePath(), ea->getLanguage()->id.c_str());
}
