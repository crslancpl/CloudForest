# Events for C++
`Event<ArgsType>` class allows us to connect global functions and member functions of classes that derived from `EventReceiver` class. Event name will have a capital 'E' prefix. If you need more arguments, create a structure to store them.
```cpp
Event<ArgsType> EMyEvent;

EMyEvent.Connect
```
# Public function
```cpp
void my_function(ArgsType args){
    //
}

EMyEvent.Connect(my_function);
```
# Member function
```cpp
class MyClass : public EventReciever {
public:
    Func(ArgsType args){
        //
    }
};

MyClass obj;
EMyEvent.Connect(&obj, (void (EventReceiver::*)(ArgsType))&EditArea::Func);
```
