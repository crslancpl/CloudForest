# Events for C++
We recommend to use an enumerator called `Event` to specify event type. The name starts with class name or namespace name. Then define `void ListenEvent()` and `void StopListenEvent()` to connect and disconnect the events. Those functions receive two parameters, `Event event` and `void (*callback)()`.
```cpp
// namespace event
namespace myns{

enum Event{
    MYNS_EVENT1,
    MYNS_EVENT2
};

void ListenEvent(Event event, void (*callback)()){
    // do something
}

void StopListenEvent(Event event, void (*callback)()){
    // do something
}

}// namespace myns
```
```cpp
// class event
class MyClass{
public:
  enum Event{
      MYCLASS_CLASS_EVENT1,
      MYCLASS_CLASS_EVENT2
  };
  
  void ListenEvent(Event event, void (*callback)()){
      // do something
  }
  
  void StopListenEvent(Event event, void (*callback)()){
      // do something
  }
};
```
All callbacks are passed as `void (*callback)()`(no parameters). Remember to change the type of callback function to match the event's parameter. For example, in `EditArea` class:
```cpp
namespace editarea{
enum Event{
    EDITAREA_CREATED,
    EDITAREA_FOCUSED_CHANGED,
    EDITAREA_LANG_CHANGED
};

void ListenEvent(Event event, void (*callback)()){
    switch(event){
    case EDITAREA_CREATED:
        new_editarea_callbacks.insert((void (*)(EditArea*))callback);
        break;
    default:
        break;
    }
}

}// namespace editarea
```
