#ifndef CORE_H_
#define CORE_H_

#include "Global.h"
#include <gtk/gtk.h>
#include <future>
#include <string>



namespace core {
const result::Result* Interact(request::Request *request);
}

#endif
