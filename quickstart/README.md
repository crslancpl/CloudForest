# Quick Start
The user interface are written with GTK4. Though C++ binding for GTK is available, we use the C version of GTK. Python is embedded to provide additional functionalities and extensions.

## Output location
Executables and libraries are generated in root directiory.

## Directories
* `datatypes` : C style structs shared across C++ files.
* `src` : C++ codes for main functionalities.
* `pythonbackend` : C++ codes using Python C API to interact between `src` folder and `pythonscripts` folder.
* `pythonscripts` : Python scripts which are required to run the app.
* `toolset` : C++ codes for helpers.
* `extension` : Extensions.
* `data` : app data, including settings and assets.

## *data* Directory:
* `styles` : for CSS files
* `syntax` : templates for syntax provider
* `ui` : GTK builder `.ui` files
