# CloudForest

>[!WARNING]
>If you are looking for a good text editor, choose other editors, this project is not done yet. It has even less functions than gedit and Notepad.


This is the source code of the code editor, using GTK4 C library in C++.
<br/>
Read the docs in [quickstart](quickstart) for an overview.
<br/>

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

>[!NOTE]
>For libCFEmbed (syntax highlighter):
>* Check out how to create a syntax template in notion doc. You can see the source code in CloudyForest repo
>* Linux version libCFEmbed.so is always the latest.
>* Windows version libCFEmbed.dll is not updated regularly.
>* Mac version is not available, but you can build it and modify the cmake file to find it.

## Available features:
* Open file/folder
* Save file
* Create file
* Drag to resize FilePanel
* Syntax highlight
* Search
* Create file in app

## Planned features:
* Language server support
* Settings
* Extension
* Blur
* Compile setting
* Embed webview
* Move widgets to an individual window

Repository created: 7/7
