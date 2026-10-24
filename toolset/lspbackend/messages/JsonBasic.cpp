#include "JsonBasic.h"
#include <cctype>
#include <cstdio>
#include <string>
#include <unistd.h>

JsonFile::JsonFile(FILE* f){
    char c;
    c = getc(f);
    printf("File start {\n");
    while ((c = getc(f)) != '}') {
        if (c == '{') {
            // another obj
            JsonObject obj(f);
        } else if (c == '[') {
            JsonArray arr(f);
        } else if(c == '\"') {
            JsonString str(f);
        }
    }
    printf("} File end\n");
}

JsonObject::JsonObject(int fd){
    char c;
    //printf("object start {\n");
    while (true) {
        read(fd, &c, 1);

        if (c == '}') {
            break;
        }

        if (c == '{') {
            // another obj
            JsonObject obj(fd);
        } else if (c == '[') {
            JsonArray arr(fd);
        } else if(c == '\"') {
            JsonString str(fd);
        }
    }
    //printf("} object end\n");
}

JsonObject::JsonObject(FILE* f){
    char c;
    printf("object start {\n");
    while ((c = getc(f)) != '}') {
        if (c == '{') {
            // another obj
            JsonObject obj(f);
        } else if (c == '[') {
            JsonArray arr(f);
        } else if(c == '\"') {
            JsonString str(f);
        }
    }
    printf("} object end\n");
}

JsonArray::JsonArray(int fd){
    char c;
    //printf("array start [\n");
    while (true) {
        read(fd, &c, 1);
        if (c  == ']') {
            break;
        }

        if (c == '{') {
            // another obj
            JsonObject obj(fd);
        } else if (c == '[') {
            JsonArray arr(fd);
        } else if(c == '\"') {
            JsonString str(fd);
        }
    }
    //printf("] array end\n");
}

JsonArray::JsonArray(FILE* f){
    char c;
    //printf("array start [\n");
    while ((c = getc(f)) != ']') {
        if (c == '{') {
            // another obj
            JsonObject obj(f);
        } else if (c == '[') {
            JsonArray arr(f);
        } else if(c == '\"') {
            JsonString str(f);
        }
    }
    //printf("] array end\n");
}

JsonString::JsonString(int fd){
    char c;
    while (true) {
        read(fd, &c, 1);
        if (c == '\"'){
            if (text.back() != '\\') {
                break;
            }
        }
        text += c;
    }
    //printf("string: %s\n", text.c_str());
}

JsonString::JsonString(FILE* f){
    char c;
    while ((c = getc(f)) != '\"') {
        text += c;
    }
    //printf("string: %s\n", text.c_str());
}


JsonNumber::JsonNumber(FILE* f){
    char c;
    while (isdigit(c = getc(f))) {
        number *= 10;
        number += c - '0';
    }

    fseek(f, -1, SEEK_CUR);
}
