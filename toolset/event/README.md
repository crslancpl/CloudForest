# Event
## Simple Event
The callbacks are stored as `EventCallback` (or `void (*) ()`)
```cpp
void SimpleEvent::Connect(EventCallback callback);

void SimpleEvent::Disconnect(EventCallback callback);

// retruns the callbacks as a set
const EventCallbacksSet &SimpleEvent::GetCallbackSet() const
```
# Usage
First create an enum `Signal`.
```cpp
enum Signal{
    SIGNAL_ONE,
    SIGNAL_TWO
}
```
Then a map mapping signals with simple event.
```cpp
std::unordered_map<Signal, SimpleEvent> event_map = {
    {SIGNAL_ONE, SimpleEvent()},
    {SIGNAL_TWO, SimpleEvent()}
}
```
Normally we define a `Listen` and `StopListen` function to connect signals.
```cpp
Listen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

StopListen(Signal signal, EventCallback callback){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}
```
Get the event from the `event_map`. And run every callback in `event.GetCallbackSet()`. The callbacks are stored as `EventCallback` (or `void (*) ()`). You might have to change the function type.
```cpp
void MyFunction(){
    // get the event
    const SimpleEvent &event = event_map.at(SIGNAL_ONE);

    // run callbacks
    for (EventCallback callback : event.GetCallbackSet()){
        // remember to convert the function type
        // assum the event doesn't have any argument
        callback()
    }
}
```
