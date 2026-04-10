#include "syntax_provider.h"


#include "datatypes/language.h"
#include "src/gui/components/TextArea.h"
#include "src/gui/editarea/EditArea_if.h"
#include "src/gui/editarea/EditArea.h"
#include "CFCore.h"

#include <unordered_map>

static std::unordered_map<TextArea*, SyntaxProvider*> provider_map;
static std::unordered_map<datatypes::Language*, SyntaxProvider*> lang_to_provider_map;

static void EditAreaCreated(EditArea* ea){
    ea->AddLangChangedCallback(syntaxprovider::SetLanguage);
}

SyntaxProvider::SyntaxProvider(datatypes::Language* language){
    m_language = language;
}

void SyntaxProvider::Highlight(TextArea* ta){

}

void syntaxprovider::Init(){
    cf::Init();
    //editarea::AddNewEditAreaCallback(EditAreaCreated);
}

void syntaxprovider::Highlight(TextArea *ta){
    provider_map.find(ta)->second->Highlight(ta);
}

void syntaxprovider::SetLanguage(TextArea *ta, datatypes::Language* language){
    auto result = lang_to_provider_map.find(language);
    SyntaxProvider *provider;
    if(result == lang_to_provider_map.end()){
        cf::LoadTemplate(language->id.c_str());
        provider = new SyntaxProvider(language);
        provider_map.emplace(ta, provider);
    }else{
        provider = result->second;
    }

    provider_map.emplace(ta, provider);
}

void syntaxprovider::FastHighlight(EditArea* ea){
    ea->ClearHighlight();
    cf::ReadFile(ea->GetFilePath(), ea->GetLanguage()->id.c_str());
}
