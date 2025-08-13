Cloudforest is a demo text editor built with GTK4, a cross-platform UI toolkit. Written in C and C++.

[GTK4](https://www.gtk.org/) is a C library, but for convenience, we are using GTK4 C library in C++. GTK does have official C++ version library called [gtkmm](https://gtkmm.gnome.org/en/index.html) , and there are also [other language bindings](https://www.gtk.org/docs/language-bindings/) , but we use C version to reduce dependency problems.

>[!IMPORTANT]
>Call by reference and call by pointer have nearly the same efficiency. But just to make it clear, function arguments/parameters in this project that is called by reference indicates that it cannot be null. Call by pointer **DOESN'T ALWAYS** indicates it can be null, since C functions only accept pointer

## Dependency
>[!IMPORTANT]
>If you are using Windows, install all these tools in msys2.

1. PkgConfig for cmake to find GTK
2. [Cmake](https://cmake.org/)
3. make, ninja or other tools (run cmake to see what you need)
4. A C++ compiler (g++ or clang++ recommende. **Visual studio compiler may not work**)
5. [GTK4](https://www.gtk.org/docs/installations/) **Some Linux distro doesn't support dev library GTK4.10**
6. [LSP Clangd](https://clangd.llvm.org/) Only clangd can find the build command (If you are pro who code without language server then ignore this)

## What you need

1. Linux/Windows/MacOS computer that you can download GTK4 **4.10 or newer**
2. A code editor that works
3. Github desktop (optional, there are Linux version on flathub )


Go to the CloudForest folder, and run `cmake .`( 'cmake' and a dot). You will see `compile_commands.json` . This helps clangd to find <gtk/gtk.h>

**After these steps you can start editing**. Now go to  [[Code]]