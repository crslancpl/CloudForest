#ifndef EVENT_H_
#define EVENT_H_

#include <unordered_set>

typedef void(*EventCallback)();// void (*)()
typedef std::unordered_set<EventCallback> EventCallbacksSet ;

class SimpleEvent{
public:
    virtual void Connect(void (*callback)());
    virtual void Disconnect(void (*callback)());

    const EventCallbacksSet &GetCallbackSet() const;

protected:
    EventCallbacksSet m_callbackSet;
};

#endif
