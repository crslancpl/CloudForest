# CloudForest

>[!WARNING]
>If you are looking for a good text editor, choose other editors, this project is not done yet. It has even less functions than gedit and Notepad.


This is the source code of the code editor, using GTK4 C library in C++. The source code is now messy.
<br/>

>[!NOTE]
>Read the documentation in [Notion](https://branch-sand-75d.notion.site/2538420fb7f28032ab16d8be325ff0b2?v=2538420fb7f2804bb28a000c6ac5fdd1) even though it is not finished yet.
<br/>

>[!NOTE]
>For libCFEmbed (syntax highlighter):
>* Check out how to create a syntax template in notion doc. You can see the source code in CloudyForest repo
>* Linux version libCFEmbed.so is always the latest.
>* Windows version libCFEmbed.dll is not updated regularly.
>* Mac version is not available, but you can build it and modify the cmake file to find it.

## Depenencies:
1. [CMake](https://cmake.org/download/)   **The minimum version is 3.5**
1. [GTK4](https://www.gtk.org/)   **Minimum GTK 4.10**
1. [Python](https://www.python.org/downloads/) **for python.h**
> [!NOTE]
> Not all OS or distro supports GTK 4.10 development package. For example apt in Debian 12 does not support libgtk-4-dev 4.10

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
