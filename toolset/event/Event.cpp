#include "Event.h"

void SimpleEvent::Connect(EventCallback callback){
    m_callbackSet.insert(callback);
}

void SimpleEvent::Disconnect(EventCallback callback){
    m_callbackSet.erase(callback);
}

const EventCallbacksSet &SimpleEvent::GetCallbackSet() const{
    return m_callbackSet;
}
