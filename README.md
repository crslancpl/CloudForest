# CloudForest

>[!WARNING]
>If you are looking for a good text editor, choose other editors, this project is not done yet. It has even less functions than gedit and Notepad.

This is the source code of the code editor, using GTK4 C library in C++. The source code is now messy.
<br/><br/>
>[!NOTE]
>Read the documentation in doc folder even though it is not finished yet. The doc is written with [obsidian](https://obsidian.md). If you have obsidian installed you can open the doc folder in obsidian

## Depenencies:
1. [CMake](https://cmake.org/download/)   **The minimum version is 3.5**
1. [GTK4](https://www.gtk.org/)   **Minimum GTK 4.10** <br><br> apt Debian 12 does not support libgtk-4-dev 4.10

## Before editing the code
1. Open a terminal in the **CloudForest** directory, and run `cmake .` This will generate the compile files, and, more importantly, the LSP command for clangd. If you skipped this step, your clangd will not be able to find `gtk/gtk.h`.

## Build
1. Run `cmake .` again to make sure the make file is right.
1. Run `make` if the cmake generates `Makefile` or `ninja` if generates `ninja build` depends on your system. Check for those files in the folder.
1. Then run the app in File Explorer or run it in terminal `./CloudForest`.

## Available features:
* Open file/folder
* Save file
* Create file
* Drag to resize FilePanel

## Planned features:
* Syntax highlight
* Language server support
* Search
* Settings
* Extension
* Create file in app
* Blur
* Compile setting
* Embed webview (I am sure webkitgtk works for linux)
* Move widgets to an individual window

>[!NOTE]
>We hope to create a basic and unified platform that links all features in the app (extensions, webview, settings, core app, lsp). Since C++ is not extensible, we may have to create a language that works like command line

Repository created: 7/7
