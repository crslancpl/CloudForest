# Loading CSS


## Load CSS from Folder
Create a folder that contains a *main.css* file.
* mystyle
  * main.css
  * file1.css
  * file2.css

In *main.css*, use `@import` to include other css files. The folder *data/styles/AppStyle* sets the basic layouts and is highly recommended to import it.

structure
* data/styles
  * AppStyle
    * main.css
  * mystyle
    * main.css

```css
/* 
 * mystyle/main.css
 */
@import url("../AppStyle/main.css")

@import url("file1.css")
@import url("file2.css")
```
In C++ source code, call `void style::LoadCssFolder(const std::string &path)`
```cpp
style::LoadCssFolder("data/styles/mystyle");
```

## Load CSS from File
Call `void style::LoadCssFile(const std::string &path)` to load the file.
```cpp
style::LoadCssFile("data/styles/mystyle.css");
```
