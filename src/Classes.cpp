#include "Classes.h"

#include <glib/gprintf.h>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <string.h>

#include "EditArea.h"
#include "FilePanel.h"



Suggestion::Suggestion(string *content, string *type, string *doc){
    Content = content;
    Type = type;
    Doc = doc;
}
