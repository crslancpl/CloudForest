#ifndef JSON_H_
#define JSON_H_

#include <string>
#include <vector>
#include <variant>

/*
 * [IMPORTANT]
 * The json for lsp is now done by python
 * See extension/CloudForestBuiltIn/LSPMsg.py
 */

class Block{
public:
    //
};

class Json{
public:
    std::vector<std::string> text;
    std::string ParseToText();
    void AddStrProp(std::string property, std::string value);
    void AddNumProp(std::string property, int value);
    void AddBlockProp(std::string property, Block *value);
};

#endif
