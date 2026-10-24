#pragma once

#include <cstdio>
#include <string>

class JsonFile {
public:
    JsonFile(int fd);
    JsonFile(FILE* f);
};

class JsonObject {
public:
    JsonObject(int fd);
    JsonObject(FILE* f);
};

class JsonArray {
public:
    JsonArray(int fd);
    JsonArray(FILE* f);
};

class JsonString {
public:
    JsonString(int fd);
    JsonString(FILE* f);

    std::string text;
};

class JsonNumber {
public:
    JsonNumber(FILE* f);

    int number;
};
