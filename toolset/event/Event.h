#ifndef EVENT_H_
#define EVENT_H_

#include <unordered_set>
#include <vector>

typedef void(*EventCallback)();// void (*)()
typedef std::unordered_set<EventCallback> EventCallbacks ;

class EventReceiver{};

//typedef void (EventReceiver::*EventReceiverCallback)(...) ;
//typedef std::vector<std::pair<EventReceiver, EventReceiverCallback>> EventReceiverCallbacks ;

class SimpleEvent{
public:
    virtual void Connect(void (*callback)());
    //virtual void Connect(EventReceiver* obj, EventReceiverCallback callback);
    virtual void Disconnect(void (*callback)());
    //virtual void DisConnect(EventReceiver* obj, EventReceiverCallback callback);

    const EventCallbacks &GetCallbackSet() const;
    //const EventReceiverCallbacks &GetEventReceiverCallbacks() const;

protected:
    EventCallbacks m_callbackSet;
    //EventReceiverCallbacks m_eventReceiverCallbackSet;
};



template<typename T_args>
class Event {
public:
    typedef void(EventReceiver::*EventCallback)(T_args);
    typedef struct {
        EventReceiver* r;
        EventCallback f;
    } MemberCallback ;

public:
    void Connect(void (*func)(T_args args)){
        m_publicFunc.insert(func);
    }

    void Disconnect(void (*func)(T_args args)){
        m_publicFunc.erase(func);
    }

    void Connect(EventReceiver* receiver, EventCallback func){
        MemberCallback cb {
            receiver, func
        };
        m_memberFunc.push_back(cb);
    }

    void Invoke(T_args args){
        for (void (*callback)(T_args) : m_publicFunc) {
            callback(args);
        }

        for (MemberCallback& cb : m_memberFunc) {
            EventReceiver* r = cb.r;
            EventCallback f = cb.f;
            ((*r).*f)(args);
        }
    }

private:
    std::unordered_set<void(*)(T_args)> m_publicFunc;
    std::vector<MemberCallback> m_memberFunc;
};

#endif
