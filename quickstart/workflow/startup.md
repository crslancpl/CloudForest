# Startup
## 1 Entry And App Creation (in *src*)
The entry function `int main()` is defined in *src/Entry.cpp*. It creates the GtkApplication. After the app is created, initalize other parts of the app. 

## 2 Initializing Syntax Provider (in *toolset*)

## 3 Starting Python Interpreter (in *pythonbackend*)
`pythonbackend::Start()` function starts the interpreter. It create `cloudforest` module and child modules. Then execute *pythonscripts/entry.py* and *extension/init.py* to add folders *pythonscripts* and *extension* to the system path.

## 4 Initializing GUI (in *src/gui*)

## 5 Initializing File Management (in *src/filemanagement*)
