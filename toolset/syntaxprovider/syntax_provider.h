#ifndef SYNTAX_PROVIDER_H_
#define SYNTAX_PROVIDER_H_

#include "datatypes/language.h"

// Forward declaration
class TextArea;
class EditArea;

class SyntaxProvider{
public:
    SyntaxProvider();
    ~SyntaxProvider();
    void Highlight(TextArea* ta);
private:
    datatypes::Language m_language;
};

namespace syntaxprovider{
    void Init();
    void SetLanguage(TextArea* ta, datatypes::Language* language);
    void Highlight(TextArea* ta);
    void FastHighlight(EditArea* ea);
}

#endif
