#ifndef CFCORE_H_
#define CFCORE_H_

#include "CFEmbed.h"
#include "../Core.h"
#include "../requests/BaseRequest.h"

namespace cf{
void Init();
void Process(Request* request);
}


#endif
