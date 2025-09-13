#ifndef CFCORE_H_
#define CFCORE_H_

#include "CFEmbed.h"
#include "../Core.h"


namespace cf{
void Init();
void Process(request::Request* request);
}


#endif
