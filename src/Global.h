#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include "Classes.h"
#include "Types.h"





namespace result{
class Result{
//nothing
};

class GetText: public Result{
public:
    std::string Text;
};

class GetAllEditAreaPath: public Result{
public:
    const std::vector<std::string>* Text;
};

}

namespace global{
extern GtkApplication *GtkApp;
}


#endif
