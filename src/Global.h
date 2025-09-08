#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <gtk/gtk.h>
#include <string>
#include <vector>

namespace results{
class Results{
public:
    void* content;
};

class GetText: public Results{
public:
    const std::string* text;
};

class GetAllEditAreaPath: public Results{
public:
    const std::vector<std::string>* text;
};
}

namespace global{
extern GtkApplication *GtkApp;
}

#endif
