#include "SessionEvent.h"
#include "toolset/event/Event.h"

#include <unordered_map>



namespace session {

static std::unordered_map<Signal, SimpleEvent> event_map = {
    {EDITAREA_CREATED, SimpleEvent()},
    {EDITAREA_FOCUSED_CHANGED, SimpleEvent()},
    {EDITAREA_LANG_CHANGED, SimpleEvent()},
    {NEW_WORKSPACE, SimpleEvent()}
};

void Listen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void StopListen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}

SimpleEvent &GetEvent(Signal signal){
    return event_map.at(signal);
}

}// namespace session
