#ifndef CORE_H_
#define CORE_H_

#include "Global.h"
#include <gtk/gtk.h>
#include <future>
#include <string>

#include "requests/BaseRequest.h"
#include "requests/CFRequests.h"
#include "requests/FileRequests.h"
#include "requests/GUIRequests.h"
#include "requests/PyRequests.h"


namespace core {
const result::Result* Interact(Request *request);
}


#endif
