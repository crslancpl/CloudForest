# Quick Start
The user interface are written with GTK4. Though C++ binding for GTK is available, we use the C version of GTK. Python is embedded to provide additional functionalities and extensions.

## Directories
The source codes are classified in 4 main directories.
* `datatypes` : C style structs shared by other C++ files.
* `src` : C++ codes for main functionalities.
* `pythonbackend` : C++ codes using Python C API to interact between `src` folder and `pythonscripts` folder.
* `pythonscripts` : Python scripts which are required to run the app.
* `toolset` : C++ codes for helpers.

Other directories:
* `styles` : for CSS files
* `syntax` : templates for syntax highlighting
* `ui` : GTK builder `.ui` files
* `data` : app data.
