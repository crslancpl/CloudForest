#include "Base.h"
#include "JsonBasic.h"
#include <cstdio>
#include <string>
#include <unistd.h>

BaseMessage::BaseMessage(FILE* fromServer) {
    char c;
    getc(fromServer);

    printf("base message started\n");
    while (true) {
        c = getc(fromServer);
        bool isKey = true; // either key or value
        //printf("c {%c}\n", c);

        if (c == '}') {
            return;
        }

        // get key and than
        if (c == '{') {
            JsonObject obj(fromServer);
        } else if (c == '[') {
            JsonArray arr(fromServer);
        } else if (c == '\"') {
            JsonString key(fromServer);
            printf("key = {%s}\n", key.text.c_str());
            if (key.text == "id") {
                JsonString str(fromServer);
            } else if (key.text == "result") {
                JsonObject obj(fromServer);
            }
        }
    }
    printf("base message ended\n");
}
