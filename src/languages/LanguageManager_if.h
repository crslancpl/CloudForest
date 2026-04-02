#ifndef LANGUAGEMANAGER_IF_H_
#define LANGUAGEMANAGER_IF_H_

namespace datatypes{
typedef struct Language Language;
}

namespace langmanager{

void Clear();
void AddLanguage(datatypes::Language lang);//called by python srcipt `pythonscripts/language/lang_loader.py"

}

#endif
