# Compilation
The top level CMakeList.txt has basic settings and packages finder. Top level CMakeList.txt does not compile anything itself, instead it includes subdirectories.

Subdirectories compilation Order:
1. *toolset* : creates TOOLSETLIB
2. *pythonbackend* : creates PYTHONBACKENDLIB
3. *src* : creates the program
