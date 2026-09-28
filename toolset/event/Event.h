#ifndef EVENT_H_
#define EVENT_H_

#include <unordered_set>
#include <vector>

typedef void(*EventCallback)();// void (*)()
typedef std::unordered_set<EventCallback> EventCallbacksSet ;

class SimpleEvent{
public:
    virtual void Connect(void (*callback)());// connect function
    virtual void Disconnect(void (*callback)());

    const EventCallbacksSet &GetCallbackSet() const;

protected:
    EventCallbacksSet m_callbackSet;
};

class EventReceiver{
    //
};

template<typename T_callback, typename T_arg>
class TypedEvent{
public:
    void Connect(T_callback callback);
    void Disconnect(T_callback callback);
    void Invoke(...);
protected:
    std::vector<T_callback> m_callbacks;
};

#endif
