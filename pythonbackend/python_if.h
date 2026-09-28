#ifndef PYTHON_IF_H_
#define PYTHON_IF_H_

typedef struct AppContext AppContext;

namespace pybackend{

void Start(AppContext& context);
void End();

AppContext& GetAppContext();

void RunEnabledExtensions();

};

#endif
