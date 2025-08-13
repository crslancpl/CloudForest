In `MainWindow.h` and `MainWindow.cpp`, `MainWindow` class is defined.

![[Screenshot from 2025-08-13 08-49-20.png]]
## MainWindow class

```cpp
class MainWindow{
public:
    EditAreaHolder *EAHolder;
    GtkWindow *Window;
    FilePanel *FP;
	HeaderBar *Headerbar;
    GtkGrid *WindowGrid;
};
```

MainWindow class is not a gtk widget. The window you see is `GtkWindow *Window` under the class.

**Core.cpp contains a variable called AppWindow. Everytime you need to get the MainWindow of the app, use GetAppWindow() and it will return the reference of AppWindow**.The loading of the variables in`MainWindow` is done by function `void NewWindow (GtkApplication *app,gpointer user_data)`. Only one `MainWindow` is need for one app so we do not create a constructor for it.

A `MainWindow` consist a `HeaderBar *Headerbar` at the top. and `GtkGrid *WindowGrid` in the middle. `WindowGrid` has a `EditAreaHolder *EAHolder` and `FilePanel *FP`. 

### UI content:
Window
     Headerbar (top)
     WindowGrid (middle)
	     FP (Left)
	     EAHolder (Right)


### CSS:
```css
#MainWindow
```


## Function

```C++
void NewWindow (GtkApplication *app, gpointer user_data);
```
It is called by Entry.cpp. It will get the `AppWindow` from `Core.cpp` with `GetAppWindow()`. And set the variables of `AppWindow`. 