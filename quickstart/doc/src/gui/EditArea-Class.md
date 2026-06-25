# EditArea class
EditArea class is an extended class from TextArea
## Events
Most of the events only pass the `EditArea` to callback functions. 
```c
void EditAreaBasicCallback(EditArea* ea)
```

### LANG_CHANGED
```cpp
void LangChangedCallback(EditArea* ea, Language* oldlang, Language* newlang)
```
### CLOSED
### COMPLETION_REQUESTED
### CURSOR_MOVED
### FILE_DATA_CHANGED
### FILE_SAVED
### LANG_CHANGED
### TEXT_CHANGED
