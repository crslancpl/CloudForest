#ifndef SYNTAX_PROVIDER_H_
#define SYNTAX_PROVIDER_H_

#include "datatypes/language.h"

// Forward declaration
class TextArea;
class EditArea;

class SyntaxProvider{
public:
    SyntaxProvider(Language *language);
    ~SyntaxProvider();
    void Highlight(TextArea* ta);
private:
    Language* m_language;
};

namespace syntaxprovider{
void Init();
void SetLanguage(TextArea* ta, Language* language);
void Highlight(TextArea* ta);
void FastHighlight(EditArea* ea);
}

#endif
