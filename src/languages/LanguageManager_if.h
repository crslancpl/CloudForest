#ifndef LANGUAGEMANAGER_IF_H_
#define LANGUAGEMANAGER_IF_H_

#include <string>

namespace datatypes{
typedef struct Language Language;
}

namespace langmanager{

void Clear();
void NewLanguage(const std::string& langname, const std::string& id, const std::string& syntaxfile);//called by python srcipt `pythonscripts/language/lang_loader.py"
datatypes::Language* FindLanguage(const char* langname);

void AddNewLangCallback(void (*callback)(datatypes::Language*));
void RemoveNewLangCallback(void (*callback)(datatypes::Language*));

}

#endif
