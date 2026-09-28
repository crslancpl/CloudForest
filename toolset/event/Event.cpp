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

template<typename T_callback, typename T_arg>
void TypedEvent<T_callback, T_arg>::Connect(T_callback callback){
    m_callbacks.push_back(callback);
}

template<typename T_callback, typename T_arg>
void TypedEvent<T_callback, T_arg>::Disconnect(T_callback callback){
    for (int i = 0; i < m_callbacks.size(); i++) {
        if (m_callbacks[i] == callback) {
            m_callbacks.erase(m_callbacks.begin() + i);
            return;
        }
    }
};

void Invoke(...);
