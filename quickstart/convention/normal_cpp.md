# C++ (without Python C API)
## File Naming
Normal source code files are named with PascalCase/CamalCap (capitalize the first letter)
```
FileName.h
FileName.cpp
```
Interfaces.
```
FileName_if.h
FileName_if.cpp
```

## Source Code Naming
For functions and methods, use CamalCap. Name the parameters/arguments with lower case.
```cpp
void NormalFunction(void* argname);
void MyClass::ClassMethod(void* argname);
```

`struct` is always with `typedef` keyword. Stuct name isn't necessary. But, if the name is defined, name it as same as the typedef. The variables inside the sturct have CamalCap name. `struct` will be treated as C. C++ struct features like **access specifiers** and **inheritance** are generally discouraged.
```cpp
// without defining struct name
typedef struct {
    void* PropertyName;
}MyStruct;
// bothstruct name and typedef
typedef struct MyStruct{
    void* PropertyName;
}MyStruct;
```

For class definition, align access specifier (`public`, `private`...) with the `class` keyword
```cpp
class MyClass : public BaseClass {
public:
    MyClass();
    ~MyClass();
    
    void PublicMethod(); 
private:
    void* m_privateProperty;
    
    void PrivateMethod();
}
```

Namespace should be named with flatcase
```cpp
namespace mynamespace{

//

}
```
