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
    GtkSeparator *Separator;
};
```

MainWindow class is not a gtk widget. The window you see is `GtkWindow *Window` under the class.

**Core.cpp contains a variable called AppWindow. Everytime you need to get the MainWindow of the app, use GetAppWindow() and it will return the reference of AppWindow**.The loading of the variables in`MainWindow` is done by function `void NewWindow (GtkApplication *app,gpointer user_data)`. Only one `MainWindow` is need for one app so we do not create a constructor for it.

A `MainWindow` consist a `HeaderBar *Headerbar` at the top. and `GtkGrid *WindowGrid` in the middle. `WindowGrid` has a `EditAreaHolder *EAHolder` and `FilePanel *FP`. 

When the user drags `Separator`. `SeparatorDragUpdate()` will be called and resize the basegrid of file panel.

### UI content:
Window
     Headerbar (top)
     WindowGrid (middle)
	     FP (Left)
	     Separator (Middle)
	     EAHolder (Right)


### CSS:
```css
#MainWindow

.Separator
```


## Function

```C++
void NewWindow (GtkApplication *app, gpointer user_data);

void SeparatorDragUpdate(GtkGestureDrag* self, gdouble x, gdouble y, gpointer user_data);
```
`NewWindow()`is called by Entry.cpp. It will get the `AppWindow` from `Core.cpp` with `GetAppWindow()`. And set the variables of `AppWindow`. 

`SeparatorDragUpdate()` is the callback function for drag gesture of `Separator`. See [GTK4 doc](https://docs.gtk.org/gtk4/signal.GestureDrag.drag-update.html) for more. It will set the width of `BaseGrid` of the file panel in the window.