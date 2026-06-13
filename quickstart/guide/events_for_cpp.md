# Events for C++
We recommend to use an enumerator called `Signal` to specify event type. The name starts with class name or namespace name. Then define `void Listen()` and `void StopListen()` to connect and disconnect the events. Those functions receive two parameters, `Signal signal` and `void (*callback)()`.
```cpp
// namespace event
namespace myns{

enum Signal{
    MYNS_EVENT1,
    MYNS_EVENT2
};

void Listen(Signal signal, void (*callback)()){
    // do something
}

void StopListen(Signal signal, void (*callback)()){
    // do something
}

}// namespace myns
```
```cpp
// class event
class MyClass{
public:
  enum Signal{
      MYCLASS_CLASS_EVENT1,
      MYCLASS_CLASS_EVENT2
  };
  
  void Listen(Signal signal, void (*callback)()){
      // do something
  }
  
  void StopListen(Signal signal, void (*callback)()){
      // do something
  }
};
```
All callbacks are passed as `void (*callback)()`(no parameters). Remember to change the type of callback function to match the event's parameter. For example, in `EditArea` class:
```cpp
namespace editarea{
enum Signal{
    EDITAREA_CREATED,
    EDITAREA_FOCUSED_CHANGED,
    EDITAREA_LANG_CHANGED
};

static std::unordered_map<Signal, SimpleEvent> event_map = {
    {EDITAREA_CREATED, SimpleEvent()},
    {EDITAREA_FOCUSED_CHANGED, SimpleEvent()},
    {EDITAREA_LANG_CHANGED, SimpleEvent()}
}

void Listen(Signal signal, void (*callback)()){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Connect(callback);
    }
}

void StopListen(Signal signal, void (*callback)()){
    auto itr = event_map.find(signal);
    if(itr != event_map.end()){
        itr->second.Disconnect(callback);
    }
}

}// namespace editarea
```
