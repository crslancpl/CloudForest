#ifndef GUIREQUEST_H_
#define GUIREQUEST_H_

#include "BaseRequest.h"

#include <string>
#include "../Types.h"

class EditArea;
//defined in gui/EditArea

class EAGetText: public Request{
public:
    /*
     * result: result::GetText
     */
    EditArea* Parent;
    //or get from filepath
    std::string Filepath;

    EAGetText() : Request(Parts::GUI) {}
};

class EASetLang: public Request{
public:
    EditArea* Parent;
    //or get from filepath
    std::string Filepath;

    std::string Lang;
    EASetLang() : Request(Parts::GUI) {}
};

class EAHideSuggestion: public Request{
public:
    EditArea* Parent;
    EAHideSuggestion():Request(Parts::GUI) {}
};

class EAShowSuggestion: public Request{
public:
    EditArea* Parent;
    EAShowSuggestion():Request(Parts::GUI) {}
};

class EAAddSuggestion: public Request{
public:
    EditArea* Parent;
    //or get from filepath
    std::string Filepath;

    Suggestion *Sug;
    EAAddSuggestion():Request(Parts::GUI) {}
};

class EAClearSuggestion: public Request{
public:
    EditArea* Parent;
    //or get from filepath
    std::string Filepath;

    EAClearSuggestion():Request(Parts::GUI) {}
};

class EADrawByPos: public Request{
    /*
     * If "Parent" is null then find the edit area by "Filepath"
     */
public:
    EditArea* Parent;
    //or get from filepath
    std::string Filepath;

    unsigned int Startpos, Endpos;
    std::string Tagname;
    EADrawByPos() : Request(Parts::GUI) {}
};

class EADrawByLine: public Request{
public:
    EditArea* Parent;
    //or get from filepath
    std::string Filepath;

    unsigned int Line, Offset, Length;
    std::string Tagname;
    EADrawByLine() : Request(Parts::GUI) {}
};

#endif
