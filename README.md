# IDE Core
This is the source code of application part of the ide.
<br/><br/>

## Depenencies:
1. [CMake](https://cmake.org/download/)   **We do not set minimum required version**
1. [GTK4](https://www.gtk.org/)
1. [WebkitGTK6](https://webkitgtk.org/)   **WebkitGTK6 is not the same with WebkitGTK2. WebkitGTK2 does not work in GTK4**

## Before editing the code
1. Open a terminal in the **IDECore** directory, and run `cmake .`. This will generate the make files, and, more importantly, the LSP command. If you skipped this step, your LSP will not be able to find `gtk/gtk.h` and `webkit/webkit.h`.

## Build
1. Run `cmake .` again to make sure the make file is right.
1. Run `make`
1. Then run the app with `./CloudForest`.



Repository created: 7/7
