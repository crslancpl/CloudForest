Setting panel is not a class because only one setting panel is needed for the app

Click on **IDE** button to show it.

Setting panel is a `GtkWindow` used as fly-out by setting decorated to `false`

## Variables
```cpp
GtkWindow *SettingWindow;
GtkButton *CloseButton;
GtkWindow *appwindow;// MainWindow
GtkBox *BaseBox;// Saperates TabButtonBox and Stack
GtkBox *TabButtonBox;// left-hand side
GtkGrid *TabButtonBoxSaperator;
GtkStack *Stack;// right-hand side
```

* `SettingWindow` is the fly-out
* `CloseButton` is shown at the bottom of `TabButtonBox`. Click it to close the fly-out
* `appwindow` is pointed to `AppWindow.window` (in Core.cpp)
* `BaseBox` is separating `TabButtonBox` and `Stack`
* `TabButtonBox` contains all tab button,  `TabButtonBoxSaperator`, and the `CloseButton`.
* `TabButtonBoxSaperator` separates tab buttons and `CloseButton`. It is pushing `CloseButton` to the bottom.