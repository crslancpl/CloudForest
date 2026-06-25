#ifndef SESSIONEVENT_H_
#define SESSIONEVENT_H_

#include "toolset/event/Event.h"

namespace session {

//
enum Signal{
    EDITAREA_CREATED,
    EDITAREA_FOCUSED_CHANGED,
    EDITAREA_LANG_CHANGED,
    LANGUAGE_USED,
    NEW_WORKSPACE
};

void Listen(Signal signal, EventCallback callback);
void StopListen(Signal signal, EventCallback callback);

SimpleEvent &GetEvent(Signal signal);

}// namespace session

#endif
